////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "rendergl40.h"

// Includes
#include <unordered_map>
#include <memory>
#include <fstream>
// OpenGL image loader
#include <SOIL2/SOIL2.h>
// Public Includes
#include <blons/math/math.h>
// Local Includes
#include "glfuncloader.h"

namespace blons
{
namespace
{
// Used to determine which context is currently active, if multiple exist
static unsigned int g_active_context;
static unsigned int g_context_count;

// Overloaded glUniforms to keep things generic
void Uniform(GLuint loc, int value)
{
    glUniform1i(loc, value);
}
void Uniform(GLuint loc, Matrix value)
{
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)value.m);
}
void Uniform(GLuint loc, Vector3 value)
{
    glUniform3fv(loc, 1, &value.x);
}
void Uniform(GLuint loc, Vector4 value)
{
    glUniform4fv(loc, 1, &value.x);
}
} // namespace

class BufferResourceGL40 : public BufferResource
{
public:
    BufferResourceGL40(RenderGL40* context) : context_(context) {}
    ~BufferResourceGL40() override;

    GLuint buffer_, vertex_array_id_;
    enum BufferType { VERTEX_BUFFER, INDEX_BUFFER } type_;
    RenderGL40* context_;
    unsigned int context_id_;
};

class TextureResourceGL40 : public TextureResource
{
public:
    TextureResourceGL40(RenderGL40* context) : context_(context) {}
    ~TextureResourceGL40() override;

    GLuint texture_;
    RenderGL40* context_;
    unsigned int context_id_;
};

class FramebufferResourceGL40 : public FramebufferResource
{
public:
    FramebufferResourceGL40(RenderGL40* context) : context_(context), depth_(TextureResourceGL40(context)) {}
    ~FramebufferResourceGL40() override;

    GLuint framebuffer_;
    units::pixel width, height;
    std::vector<TextureResourceGL40> targets_;
    TextureResourceGL40 depth_;
    GLuint depth_render_;
    RenderGL40* context_;
    unsigned int context_id_;
};

class ShaderResourceGL40 : public ShaderResource
{
public:
    ShaderResourceGL40(RenderGL40* context) : context_(context) {}
    ~ShaderResourceGL40() override;

    GLuint program_;
    GLuint vertex_shader_;
    GLuint frag_shader_;
    RenderGL40* context_;
    unsigned int context_id_;

    GLint UniformLocation(const char* name);
    template <typename T>
    bool SetUniform(const char* name, T value);

private:
    struct HashFunc { unsigned int operator()(const char* s){ return FastHash(s); } };
    struct CompFunc { bool operator()(const char* a, const char* b) { return strcmp(a, b) == 0; } };
    std::unordered_map<const char*, GLint, HashFunc, CompFunc> uniform_location_cache_;
    // slow, but clean. possible memory leak with const char* key anyway, iduno
    // std::unordered_map<std::string, GLint> uniform_location_cache_;
};

BufferResourceGL40::~BufferResourceGL40()
{
    if (context_id_ != g_active_context)
    {
        // TODO: Something cleaner in case owning context isnt actually deleted
        return;
    }

    context_->UnmapBuffers();

    if (type_ == BufferResourceGL40::VERTEX_BUFFER)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else if (type_ == BufferResourceGL40::INDEX_BUFFER)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glDeleteBuffers(1, &buffer_);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vertex_array_id_);
}

FramebufferResourceGL40::~FramebufferResourceGL40()
{
    if (context_id_ != g_active_context)
    {
        return;
    }

    glDeleteRenderbuffers(1, &depth_render_);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &framebuffer_);
}

TextureResourceGL40::~TextureResourceGL40()
{
    if (context_id_ != g_active_context)
    {
        return;
    }

    glDeleteTextures(1, &texture_);
}

ShaderResourceGL40::~ShaderResourceGL40()
{
    if (context_id_ != g_active_context)
    {
        return;
    }

    glDetachShader(program_, vertex_shader_);
    glDetachShader(program_, frag_shader_);

    glDeleteShader(vertex_shader_);
    glDeleteShader(frag_shader_);

    glDeleteProgram(program_);

    context_->UnbindShader();
}

GLint ShaderResourceGL40::UniformLocation(const char* name)
{
    auto it = uniform_location_cache_.find(name);
    if (it == uniform_location_cache_.end())
    {
        auto location = glGetUniformLocation(program_, name);
        uniform_location_cache_[_strdup(name)] = location;
        return location;
    }
    return it->second;
}

template <typename T>
bool ShaderResourceGL40::SetUniform(const char* name, T value)
{
    // Clear errors so we know problems are isolated to this function
    glGetError();

    context_->BindShader(program_);

    auto location = UniformLocation(name);
    if (location < 0)
    {
        return false;
    }
    Uniform(location, value);
    if (glGetError() != GL_NO_ERROR)
    {
        return false;
    }
    return true;
}

RenderGL40::RenderGL40(Client::Info screen_info, bool vsync, bool fullscreen)
{
    g_context_count++;
    id_ = g_context_count;
    g_active_context = id_;

    // Mitigates repeated calls to glUseProgram
    active_shader_ = 0;

    screen_ = screen_info;

    // Defining the pixel format we want OpenGL to use
    const int color_depth = 24;
    const int depth_bits = 24;
    const int stencil_bits = 8;
    const int aux_buffers = 0;

    // Time to create a dummy window, to create a false render context,
    // to create a better window with a better context...
    WNDCLASSEX wc = {};
    auto hinstance = GetModuleHandle(nullptr);
    auto app_name = L"blonstech dummy window";

    // Init dummy window class
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = app_name;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);
    auto dummy_hwnd = CreateWindowEx(WS_EX_APPWINDOW, app_name, app_name, WS_POPUP,
                                     0, 0, 640, 480, nullptr, nullptr, hinstance, nullptr);
    if (dummy_hwnd == nullptr)
    {
        throw "Failed dummy initialization";
    }

    // We dont want the world to see us like this
    ShowWindow(dummy_hwnd, SW_HIDE);

    auto dummy_device_context = GetDC(dummy_hwnd);

    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        color_depth,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        depth_bits,
        stencil_bits,
        aux_buffers,
        PFD_MAIN_PLANE,
        0, 0, 0, 0
    };
    int pixel_format = ChoosePixelFormat(dummy_device_context, &pfd);
    if (!pixel_format || !SetPixelFormat(dummy_device_context, pixel_format, &pfd))
    {
        throw "Failed dummy pixel format initialization";
    }

    // Creating a false context, to get gl functions, to create a better context...
    auto dummy_render_context = wglCreateContext(dummy_device_context);
    wglMakeCurrent(dummy_device_context, dummy_render_context);

    // TODO: actually check we get the needed functions
    if (LoadWGLFunctions().size() > 0)
    {
        throw "Failed to load OpenGL proc addresses";
    }

    // Clean up dummy context
    wglDeleteContext(dummy_render_context);
    ReleaseDC(dummy_hwnd, dummy_device_context);
    DestroyWindow(dummy_hwnd);

    // Get the real device context handle
    device_context_ = GetDC(screen_.hwnd);

    // Set the proper pixel format
    unsigned int num_formats;
    const int pixel_attributes[] =
    {
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, color_depth,
        WGL_DEPTH_BITS_ARB, depth_bits,
        WGL_STENCIL_BITS_ARB, stencil_bits,
        0
    };
    int result = wglChoosePixelFormatARB(device_context_, pixel_attributes, nullptr, 1, &pixel_format, &num_formats);
    if (!result || !SetPixelFormat(device_context_, pixel_format, &pfd))
    {
        throw "Failed to set pixel format";
    }

    // Create the real context
    const int context_attributes[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 0,
        0
    };
    render_context_ = wglCreateContextAttribsARB(device_context_, 0, context_attributes);

    if (render_context_ == nullptr)
    {
        throw "Failed to create context";
    }

    // Clean up the dummy context
    if (!wglMakeCurrent(device_context_, render_context_))
    {
        throw "Failed to set context as current";
    }

    // Finally load the rest of our functions
    if (LoadGLFunctions().size() > 0)
    {
        throw "Failed to load OpenGL proc addresses";
    }

    // Grab video card info
    video_card_desc_ = (char*)glGetString(GL_VENDOR);
    video_card_desc_ += " ";
    video_card_desc_ += (char*)glGetString(GL_RENDERER);
    // This isn't supported in OpenGL
    video_card_memory_ = 0;

    // Enable depth testing, with a default of 1.0
    depth_testing_ = true;
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    // Configure how we render tris
    // TODO: re enable back face cullin
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    //glDisable(GL_CULL_FACE);

    // Enable transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Configure vsync (please be false)
    vsync_ = vsync;
    if (!wglSwapIntervalEXT(vsync))
    {
        throw "Failed to configure vertical sync";
    }
}

RenderGL40::~RenderGL40()
{
    // TODO: Somehow nullify all created resources in here
    // Reset the current context before deleting it
    wglMakeCurrent(device_context_, nullptr);
    wglDeleteContext(render_context_);

    g_active_context = 0;
}

void RenderGL40::BeginScene()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderGL40::EndScene()
{
    SwapBuffers(device_context_);
}

BufferResource* RenderGL40::MakeBufferResource()
{
    return new BufferResourceGL40(this);
}

FramebufferResource* RenderGL40::MakeFramebufferResource()
{
    return new FramebufferResourceGL40(this);
}

TextureResource* RenderGL40::MakeTextureResource()
{
    return new TextureResourceGL40(this);
}

ShaderResource* RenderGL40::MakeShaderResource()
{
    return new ShaderResourceGL40(this);
}

bool RenderGL40::Register3DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned int* indices, unsigned int index_count)
{
    BufferResourceGL40* vertex_buf = static_cast<BufferResourceGL40*>(vertex_buffer);
    BufferResourceGL40* index_buf = static_cast<BufferResourceGL40*>(index_buffer);

    // Set the buffer types
    vertex_buf->type_ = BufferResourceGL40::VERTEX_BUFFER;
    index_buf->type_ = BufferResourceGL40::INDEX_BUFFER;

    // Generate a vertex array and set it
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    vertex_buf->vertex_array_id_ = index_buf->vertex_array_id_ = vertex_array_id;
    glBindVertexArray(vertex_array_id);

    // Attach vertex buffer data to VAO
    glGenBuffers(1, &vertex_buf->buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(Vertex), vertices, GL_STATIC_DRAW);

    // Enable vertex inputs
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    // Layout the Vertex struct type to gpu vertex attributes
    // Position declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // UV declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
    // Normal declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5*sizeof(float)));
    // Tangent declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8*sizeof(float)));
    // Bitangent declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11*sizeof(float)));

    // Setup the index buffer
    glGenBuffers(1, &index_buf->buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf->buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // nvogl32.dll loves it when i clean up my VAOs!
    glBindVertexArray(0);

    return true;
}

bool RenderGL40::Register2DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned int* indices, unsigned int index_count)
{
    BufferResourceGL40* vertex_buf = static_cast<BufferResourceGL40*>(vertex_buffer);
    BufferResourceGL40* index_buf = static_cast<BufferResourceGL40*>(index_buffer);

    // Set the buffer types
    vertex_buf->type_ = BufferResourceGL40::VERTEX_BUFFER;
    index_buf->type_ = BufferResourceGL40::INDEX_BUFFER;

    // Generate a vertex array and set it
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    vertex_buf->vertex_array_id_ = index_buf->vertex_array_id_ = vertex_array_id;
    glBindVertexArray(vertex_array_id);

    // Attach vertex buffer data to VAO
    glGenBuffers(1, &vertex_buf->buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);

    // Enable pos and uv inputs ??
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Layout the Vertex struct type to gpu vertex attributes
    // Position declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // UV declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));

    // Setup the index buffer
    glGenBuffers(1, &index_buf->buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf->buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);

    // nvogl32.dll loves it when i clean up my VAOs!
    glBindVertexArray(0);

    return true;
}

bool RenderGL40::RegisterFramebuffer(FramebufferResource* frame_buffer,
                                     units::pixel width, units::pixel height,
                                     std::vector<TextureHint> formats, bool store_depth)
{
    FramebufferResourceGL40* fbo = static_cast<FramebufferResourceGL40*>(frame_buffer);

    fbo->width = width;
    fbo->height = height;

    // Create the frame buffer
    glGenFramebuffers(1, &fbo->framebuffer_);

    // Bind it so we can put stuff in it
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->framebuffer_);

    // Creates empty render targets
    auto make_texture = [&](TextureHint hint, bool is_depth)
    {
        TextureResourceGL40 tex(this);

        // Generate an ID for the texture.
        glGenTextures(1, &tex.texture_);

        // Bind the texture as a 2D texture.
        glBindTexture(GL_TEXTURE_2D, tex.texture_);

        // Set the parameters and fill it with empty image data
        if (!is_depth)
        {
            GLint internalformat;
            switch (hint.format)
            {
            case TextureHint::R16G16:
                internalformat = GL_RG16;
                break;
            case TextureHint::NONE:
            case TextureHint::R8G8B8:
            default:
                internalformat = GL_RGB;
                break;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
         
        // Always rendered as unstretched 2D for now...
        switch (hint.filter)
        {
        case TextureHint::NEAREST:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            break;
        case TextureHint::LINEAR:
        default:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;
        }

        return tex;
    };

    // Create and bind all of our render targets
    std::unique_ptr<GLenum[]> drawbuffers(new GLenum[formats.size()]);
    for (unsigned int i = 0; i < formats.size(); i++)
    {
        if (formats[i].format != TextureHint::NONE)
        {
            fbo->targets_.push_back(make_texture(formats[i], false));
            auto attachment = GL_COLOR_ATTACHMENT0 + i;
            glFramebufferTexture(GL_FRAMEBUFFER, attachment, fbo->targets_.back().texture_, 0);
            drawbuffers[i] = attachment;
        }
    }
    glDrawBuffers(static_cast<GLsizei>(formats.size()), drawbuffers.get());

    if (store_depth)
    {
        // Render with a depth buffer
        glGenRenderbuffers(1, &fbo->depth_render_);
        glBindRenderbuffer(GL_RENDERBUFFER, fbo->depth_render_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->depth_render_);

        // Create and bind the depth target
        fbo->depth_ = make_texture({ TextureHint::NONE, TextureHint::LINEAR }, true);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo->depth_.texture_, 0);
    }

    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

bool RenderGL40::RegisterTexture(TextureResource* texture, PixelData* pixel_data)
{
    TextureResourceGL40* tex = static_cast<TextureResourceGL40*>(texture);

    // Generate an ID for the texture.
    glGenTextures(1, &tex->texture_);

    // Bind the texture as a 2D texture.
    glBindTexture(GL_TEXTURE_2D, tex->texture_);

    // Load the texture onto GPU
    unsigned int soil_flags = SOIL_FLAG_TEXTURE_REPEATS;
    if (pixel_data->format == PixelData::DDS)
    {
        soil_flags |= SOIL_FLAG_DDS_LOAD_DIRECT;
    }
    else if (pixel_data->format == PixelData::AUTO)
    {
        soil_flags |= SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_GL_MIPMAPS;
    }
    int channels = 0;
    switch (pixel_data->bits)
    {
    case PixelData::A8:
        channels = 1;
        break;
    case PixelData::R8G8B8:
        channels = 3;
        break;
    case PixelData::R8G8B8A8:
        channels = 4;
        break;
    default:
        return false;
    }
    tex->texture_ = SOIL_create_OGL_texture(pixel_data->pixels.get(), &pixel_data->width, &pixel_data->height,
                                            channels, tex->texture_, soil_flags);
    if (tex->texture_ == 0)
    {
        return false;
    }

    // Apply our texture settings (we do this after to override SOIL settings)
    // Set the texture to repeat when sampled outside UV range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Raw textures use nearest neighbour filtering
    if (pixel_data->format == PixelData::RAW)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    // TODO: attach this to a setting + safety check for max
    // glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &float);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

    // TODO: Re enable this for non-dds textures
    // glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}

bool RenderGL40::RegisterShader(ShaderResource* program,
                                std::string vertex_filename, std::string pixel_filename,
                                ShaderAttributeList inputs)
{
    ShaderResourceGL40* shader = static_cast<ShaderResourceGL40*>(program);

    // Load the shader files into memory
    std::ifstream vert_file(vertex_filename, std::ios::binary);
    vert_file.imbue(std::locale("C"));
    std::string vert_bytes((std::istreambuf_iterator<char>(vert_file)), 
                            std::istreambuf_iterator<char>());
    vert_file.close();
    std::ifstream frag_file(pixel_filename, std::ios::binary);
    frag_file.imbue(std::locale("C"));
    std::string frag_bytes((std::istreambuf_iterator<char>(frag_file)), 
                            std::istreambuf_iterator<char>());
    frag_file.close();
    if (!vert_bytes.size() || !frag_bytes.size())
    {
        return false;
    }

    // Initialize and compile the shaders
    shader->vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
    shader->frag_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
    const char* vb = vert_bytes.data();
    const char* fb = frag_bytes.data();
    glShaderSource(shader->vertex_shader_, 1, &vb, nullptr);
    glShaderSource(shader->frag_shader_, 1, &fb, nullptr);
    glCompileShader(shader->vertex_shader_);
    glCompileShader(shader->frag_shader_);

    // Check that everything went OK
    int vert_result, frag_result;
    glGetShaderiv(shader->vertex_shader_, GL_COMPILE_STATUS, &vert_result);
    if (!vert_result)
    {
        LogCompileErrors(shader->vertex_shader_, true);
        return false;
    }
    glGetShaderiv(shader->frag_shader_, GL_COMPILE_STATUS, &frag_result);
    if (!frag_result)
    {
        LogCompileErrors(shader->frag_shader_, true);
        return false;
    }

    // Take our shaders and turn it into a render pipeline
    shader->program_ = glCreateProgram();
    glAttachShader(shader->program_, shader->vertex_shader_);
    glAttachShader(shader->program_, shader->frag_shader_);
    for (const auto& input : inputs)
    {
        glBindAttribLocation(shader->program_, input.first, input.second.c_str());
    }
    glLinkProgram(shader->program_);

    // Check that everything went OK
    int link_result;
    glGetProgramiv(shader->program_, GL_LINK_STATUS, &link_result);
    if (!link_result)
    {
        LogCompileErrors(shader->program_, false);
        return false;
    }

    return true;
}

void RenderGL40::RenderShader(ShaderResource* program, unsigned int index_count)
{
    UnmapBuffers();

    ShaderResourceGL40* shader = static_cast<ShaderResourceGL40*>(program);

    BindShader(shader->program_);

    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    // TODO: make this only called once per shader somehow
    // nvogl32.dll loves it when i clean up my VAOs!
    glBindVertexArray(0);
}

void RenderGL40::BindFramebuffer(FramebufferResource* frame_buffer)
{
    if (frame_buffer != nullptr)
    {
        FramebufferResourceGL40* fbo = static_cast<FramebufferResourceGL40*>(frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->framebuffer_);
        glViewport(0, 0, fbo->width, fbo->height);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screen_.width, screen_.height);
    }
}

std::vector<const TextureResource*> RenderGL40::FramebufferTextures(FramebufferResource* frame_buffer)
{
    FramebufferResourceGL40* fbo = static_cast<FramebufferResourceGL40*>(frame_buffer);

    std::vector<const TextureResource*> targets;
    for (const auto& tex : fbo->targets_)
    {
        targets.push_back(&tex);
    }

    return targets;
}

const TextureResource* RenderGL40::FramebufferDepthTexture(FramebufferResource* frame_buffer)
{
    FramebufferResourceGL40* fbo = static_cast<FramebufferResourceGL40*>(frame_buffer);

    return &fbo->depth_;
}

void RenderGL40::BindMeshBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)
{
    UnmapBuffers();

    BufferResourceGL40* vertex_buf = static_cast<BufferResourceGL40*>(vertex_buffer);
    glBindVertexArray(vertex_buf->vertex_array_id_);
}

void RenderGL40::SetMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                             const Vertex* vertices, unsigned int vert_count,
                             const unsigned int* indices, unsigned int index_count)
{
    BufferResourceGL40* vertex_buf = static_cast<BufferResourceGL40*>(vertex_buffer);
    BufferResourceGL40* index_buf = static_cast<BufferResourceGL40*>(index_buffer);

    glBindVertexArray(vertex_buf->vertex_array_id_);
    // Attach vertex buffer data to VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    // Use GL_DYNAMIC_DRAW as these vertex buffers are updated often to allow sprite movement
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);

    // Attach index buffer data to VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf->buffer_);
    // Use GL_DYNAMIC_DRAW as these vertex buffers are updated often to allow sprite movement
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
}

void RenderGL40::UpdateMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                const Vertex* vertices, unsigned int vert_offset, unsigned int vert_count,
                                const unsigned int* indices, unsigned int index_offset, unsigned int index_count)
{
    BufferResourceGL40* vertex_buf = static_cast<BufferResourceGL40*>(vertex_buffer);
    BufferResourceGL40* index_buf  = static_cast<BufferResourceGL40*>(index_buffer);

    glBindVertexArray(vertex_buf->vertex_array_id_);
    // Attach vertex buffer data to VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glBufferSubData(GL_ARRAY_BUFFER, vert_offset * sizeof(Vertex), vert_count * sizeof(Vertex), vertices);

    // Attach index buffer data to VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf->buffer_);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset * sizeof(unsigned int), index_count * sizeof(unsigned int), indices);
}

void RenderGL40::MapMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                   Vertex** vertex_data, unsigned int** index_data)
{
    BufferResourceGL40* vertex_buf = static_cast<BufferResourceGL40*>(vertex_buffer);
    BufferResourceGL40* index_buf  = static_cast<BufferResourceGL40*>(index_buffer);

    if (vertex_buf->buffer_ != mapped_buffers_.vertex || index_buf->buffer_ != mapped_buffers_.index)
    {
        if (mapped_buffers_.vertex != 0 || mapped_buffers_.index != 0)
        {
            UnmapBuffers();
        }
    }
    else
    {
        *vertex_data = mapped_buffers_.vertex_data;
        *index_data = mapped_buffers_.index_data;
        return;
    }
    mapped_buffers_.vertex = vertex_buf->buffer_;
    mapped_buffers_.index = index_buf->buffer_;

    glBindVertexArray(vertex_buf->vertex_array_id_);

    glBindBuffer(GL_ARRAY_BUFFER, mapped_buffers_.vertex);
    *vertex_data = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
    mapped_buffers_.vertex_data = *vertex_data;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapped_buffers_.index);
    *index_data = static_cast<unsigned int*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE));
    mapped_buffers_.index_data = *index_data;
}

bool RenderGL40::SetShaderInput(ShaderResource* program, const char* name, const int value)
{
    auto prog = static_cast<ShaderResourceGL40*>(program);
    return prog->SetUniform(name, value);
}

bool RenderGL40::SetShaderInput(ShaderResource* program, const char* name, const Matrix value)
{
    auto prog = static_cast<ShaderResourceGL40*>(program);
    return prog->SetUniform(name, value);
}

bool RenderGL40::SetShaderInput(ShaderResource* program, const char* name, const Vector3 value)
{
    auto prog = static_cast<ShaderResourceGL40*>(program);
    return prog->SetUniform(name, value);
}

bool RenderGL40::SetShaderInput(ShaderResource* program, const char* name, const Vector4 value)
{
    auto prog = static_cast<ShaderResourceGL40*>(program);
    return prog->SetUniform(name, value);
}

bool RenderGL40::SetShaderInput(ShaderResource* program, const char* name, const TextureResource* value, unsigned int texture_index)
{
    const TextureResourceGL40* tex = static_cast<const TextureResourceGL40*>(value);
    glActiveTexture(GL_TEXTURE0 + texture_index);
    glBindTexture(GL_TEXTURE_2D, tex->texture_);
    return SetShaderInput(program, name, texture_index);
}

bool RenderGL40::SetDepthTesting(bool enable)
{
    if (enable == depth_testing_)
    {
        return true;
    }

    depth_testing_ = enable;
    if (enable)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    return true;
}

void RenderGL40::VideoCardInfo(char* name, int& memory)
{
    strcpy_s(name, 128, video_card_desc_.c_str());
    memory = video_card_memory_;
    return;
}

bool RenderGL40::LoadPixelData(std::string filename, PixelData* data)
{
    std::string filetype(filename);
    int channels = 0;
    unsigned char* pixel_data = SOIL_load_image(filename.c_str(), &data->width, &data->height,
                                                &channels, SOIL_LOAD_AUTO);
    if (pixel_data == nullptr)
    {
        return false;
    }
    data->pixels.reset(pixel_data);

    filetype = filetype.substr(filetype.size() - 4);

    if (filetype == ".dds")
    {
        data->format = PixelData::DDS;
    }
    else
    {
        data->format = PixelData::AUTO;
    }

    switch (channels)
    {
    case 1:
        data->bits = PixelData::A8;
        break;
    case 3:
        data->bits = PixelData::R8G8B8;
        break;
    case 4:
        data->bits = PixelData::R8G8B8A8;
        break;
    default:
        data->bits = PixelData::R8G8B8A8;
        break;
    }
    return true;
}

void RenderGL40::BindShader(GLuint shader)
{
    // Avoid repeated calls to glUseProgram (perf boost)
    if (shader != active_shader_)
    {
        glUseProgram(shader);
    }
    active_shader_ = shader;
}

void RenderGL40::UnbindShader()
{
    active_shader_ = 0;
}

void RenderGL40::UnmapBuffers()
{
    if (mapped_buffers_.vertex != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mapped_buffers_.vertex);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    if (mapped_buffers_.index != 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapped_buffers_.index);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }
    mapped_buffers_.vertex = 0;
    mapped_buffers_.index = 0;
}

void RenderGL40::LogCompileErrors(GLuint resource, bool is_shader)
{
    int buffer_size;
    if (is_shader)
    {
        glGetShaderiv(resource, GL_INFO_LOG_LENGTH, &buffer_size);
    }
    else
    {
        glGetProgramiv(resource, GL_INFO_LOG_LENGTH, &buffer_size);
    }
    buffer_size++;
    std::unique_ptr<char[]> compile_errors(new char[buffer_size]);
    if (is_shader)
    {
        glGetShaderInfoLog(resource, buffer_size, nullptr, compile_errors.get());
    }
    else
    {
        glGetProgramInfoLog(resource, buffer_size, nullptr, compile_errors.get());
    }
    log::Debug("----- SHADER COMPILATION LOG -------------------------\n");
    log::Debug(compile_errors.get());
    log::Debug("------------------------------------------------------\n");

    return;
}
} // namespace blons
