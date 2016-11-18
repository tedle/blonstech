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
#include "renderergl43.h"

// Includes
#include <unordered_map>
#include <memory>
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
// Overloaded glUniforms to keep things generic
void Uniform(GLuint loc, float value)
{
    glUniform1f(loc, value);
}
void Uniform(GLuint loc, int value)
{
    glUniform1i(loc, value);
}
void Uniform(GLuint loc, Matrix value)
{
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)value.m);
}
void Uniform(GLuint loc, Vector2 value)
{
    glUniform2fv(loc, 1, &value.x);
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

class BufferResourceGL43 : public BufferResource
{
public:
    BufferResourceGL43(Renderer::ContextID parent_id) : parent_id_(parent_id) {}
    ~BufferResourceGL43() override;

    GLuint buffer_, vertex_array_id_;
    enum BufferType { VERTEX_BUFFER, INDEX_BUFFER } type_;
    const Renderer::ContextID parent_id_;
};

class TextureResourceGL43 : public TextureResource
{
public:
    TextureResourceGL43(Renderer::ContextID parent_id) : parent_id_(parent_id) {}
    ~TextureResourceGL43() override;

    GLuint texture_;
    const Renderer::ContextID parent_id_;
    GLint type_; ///< GL_TEXTURE_2D or GL_TEXTURE_3D
};

class FramebufferResourceGL43 : public FramebufferResource
{
public:
    FramebufferResourceGL43(Renderer::ContextID parent_id) : parent_id_(parent_id) {}
    ~FramebufferResourceGL43() override;

    GLuint framebuffer_;
    units::pixel width, height;
    std::vector<std::unique_ptr<TextureResourceGL43>> targets_;
    std::unique_ptr<TextureResourceGL43> depth_;
    GLuint depth_render_;
    const Renderer::ContextID parent_id_;
};

class ShaderResourceGL43 : public ShaderResource
{
public:
    ShaderResourceGL43(Renderer::ContextID parent_id) : parent_id_(parent_id) {}
    ~ShaderResourceGL43() override;

    GLuint program_;
    std::vector<GLuint> shaders_;
    const Renderer::ContextID parent_id_;
    enum ShaderType { NONE, PIPELINE, COMPUTE } type_;

    GLint UniformLocation(const char* name);
    template <typename T>
    bool SetUniform(const char* name, T value);

private:
    struct HashFunc { unsigned int operator()(const char* s) const { return FastHash(s); } };
    struct CompFunc { bool operator()(const char* a, const char* b) const { return strcmp(a, b) == 0; } };
    std::unordered_map<const char*, GLint, HashFunc, CompFunc> uniform_location_cache_;
    // slow, but clean. possible memory leak with const char* key anyway, iduno
    // std::unordered_map<std::string, GLint> uniform_location_cache_;
};

BufferResourceGL43::~BufferResourceGL43()
{
    auto active_context = render::context();
    // TODO: Something cleaner in case owning context isnt actually deleted (inactive?)
    // This is save when deleted since OpenGL contexts clean up after themselves
    if (parent_id_ != active_context->id())
    {
        return;
    }

    auto context = static_cast<RendererGL43*>(active_context);
    context->UnmapBuffers();

    if (type_ == BufferResourceGL43::VERTEX_BUFFER)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else if (type_ == BufferResourceGL43::INDEX_BUFFER)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glDeleteBuffers(1, &buffer_);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vertex_array_id_);
}

FramebufferResourceGL43::~FramebufferResourceGL43()
{
    auto active_context = render::context();
    if (parent_id_ != active_context->id())
    {
        return;
    }

    glDeleteRenderbuffers(1, &depth_render_);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &framebuffer_);
}

TextureResourceGL43::~TextureResourceGL43()
{
    auto active_context = render::context();
    if (parent_id_ != active_context->id())
    {
        return;
    }

    glDeleteTextures(1, &texture_);
}

ShaderResourceGL43::~ShaderResourceGL43()
{
    auto active_context = render::context();
    if (parent_id_ != active_context->id())
    {
        return;
    }

    auto context = static_cast<RendererGL43*>(active_context);

    for (const auto& shader : shaders_)
    {
        glDetachShader(program_, shader);
        glDeleteShader(shader);
    }

    glDeleteProgram(program_);

    context->UnbindShader();
}

GLint ShaderResourceGL43::UniformLocation(const char* name)
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
bool ShaderResourceGL43::SetUniform(const char* name, T value)
{
    // Clear errors so we know problems are isolated to this function
    glGetError();

    auto context = static_cast<RendererGL43*>(render::context());
    context->BindShader(program_);

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

RendererGL43::RendererGL43(Client::Info screen_info, bool vsync, bool fullscreen)
{
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
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
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

    // Set the row padding on textures to be 1 (default 4)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Configure vsync (please be false)
    vsync_ = vsync;
    if (!wglSwapIntervalEXT(vsync))
    {
        throw "Failed to configure vertical sync";
    }
}

RendererGL43::~RendererGL43()
{
    // TODO: Somehow nullify all created resources in here
    // Reset the current context before deleting it
    wglMakeCurrent(device_context_, nullptr);
    wglDeleteContext(render_context_);
}

void RendererGL43::BeginScene(Vector4 clear_colour)
{
    glClearColor(clear_colour.r, clear_colour.g, clear_colour.b, clear_colour.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererGL43::EndScene()
{
    SwapBuffers(device_context_);
}

BufferResource* RendererGL43::MakeBufferResource()
{
    return new BufferResourceGL43(id());
}

FramebufferResource* RendererGL43::MakeFramebufferResource()
{
    return new FramebufferResourceGL43(id());
}

TextureResource* RendererGL43::MakeTextureResource()
{
    return new TextureResourceGL43(id());
}

ShaderResource* RendererGL43::MakeShaderResource()
{
    return new ShaderResourceGL43(id());
}

bool RendererGL43::Register3DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned int* indices, unsigned int index_count)
{
    BufferResourceGL43* vertex_buf = static_cast<BufferResourceGL43*>(vertex_buffer);
    BufferResourceGL43* index_buf = static_cast<BufferResourceGL43*>(index_buffer);

    // Set the buffer types
    vertex_buf->type_ = BufferResourceGL43::VERTEX_BUFFER;
    index_buf->type_ = BufferResourceGL43::INDEX_BUFFER;

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
    glEnableVertexAttribArray(5);

    // Layout the Vertex struct type to gpu vertex attributes
    // Position declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // UV declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
    // Lightmap UV declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(LIGHT_TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5*sizeof(float)));
    // Normal declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7*sizeof(float)));
    // Tangent declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(10*sizeof(float)));
    // Bitangent declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(13*sizeof(float)));

    // Setup the index buffer
    glGenBuffers(1, &index_buf->buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf->buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // nvogl32.dll loves it when i clean up my VAOs!
    glBindVertexArray(0);

    return true;
}

bool RendererGL43::Register2DMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned int* indices, unsigned int index_count)
{
    BufferResourceGL43* vertex_buf = static_cast<BufferResourceGL43*>(vertex_buffer);
    BufferResourceGL43* index_buf = static_cast<BufferResourceGL43*>(index_buffer);

    // Set the buffer types
    vertex_buf->type_ = BufferResourceGL43::VERTEX_BUFFER;
    index_buf->type_ = BufferResourceGL43::INDEX_BUFFER;

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
    glVertexAttribPointer(POS, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // UV declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));

    // Setup the index buffer
    glGenBuffers(1, &index_buf->buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf->buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);

    // nvogl32.dll loves it when i clean up my VAOs!
    glBindVertexArray(0);

    return true;
}

bool RendererGL43::RegisterFramebuffer(FramebufferResource* frame_buffer,
                                     units::pixel width, units::pixel height,
                                     std::vector<TextureType> formats, bool store_depth)
{
    FramebufferResourceGL43* fbo = static_cast<FramebufferResourceGL43*>(frame_buffer);

    fbo->width = width;
    fbo->height = height;

    // Create the frame buffer
    glGenFramebuffers(1, &fbo->framebuffer_);

    // Bind it so we can put stuff in it
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->framebuffer_);

    // Creates empty render targets
    auto make_texture = [&](TextureType type)
    {
        auto tex = std::make_unique<TextureResourceGL43>(id());

        // Generate an ID for the texture.
        glGenTextures(1, &tex->texture_);

        PixelData pixels;
        pixels.type = type;
        pixels.width = width;
        pixels.height = height;
        pixels.pixels = nullptr;
        SetTextureData(tex.get(), &pixels);

        return tex;
    };

    // Create and bind all of our render targets
    std::unique_ptr<GLenum[]> drawbuffers(new GLenum[formats.size()]);
    for (unsigned int i = 0; i < formats.size(); i++)
    {
        if (formats[i].format != TextureType::NONE)
        {
            fbo->targets_.push_back(make_texture(formats[i]));
            auto attachment = GL_COLOR_ATTACHMENT0 + i;
            glFramebufferTexture(GL_FRAMEBUFFER, attachment, fbo->targets_.back()->texture_, 0);
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
        fbo->depth_ = make_texture({ TextureType::DEPTH, TextureType::LINEAR, TextureType::CLAMP });
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo->depth_->texture_, 0);
    }

    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

bool RendererGL43::RegisterTexture(TextureResource* texture, PixelData* pixel_data)
{
    TextureResourceGL43* tex = static_cast<TextureResourceGL43*>(texture);
    tex->type_ = GL_TEXTURE_2D;

    // Generate an ID for the texture.
    glGenTextures(1, &tex->texture_);

    // Bind the texture as a 2D texture.
    glBindTexture(tex->type_, tex->texture_);

    // Upload image data to the GPU
    SetTextureData(tex, pixel_data);
    return true;
}

bool RendererGL43::RegisterTexture(TextureResource* texture, PixelData3D* pixel_data)
{
    TextureResourceGL43* tex = static_cast<TextureResourceGL43*>(texture);
    tex->type_ = GL_TEXTURE_3D;

    // Generate an ID for the texture.
    glGenTextures(1, &tex->texture_);

    // Bind the texture as a 2D texture.
    glBindTexture(tex->type_, tex->texture_);

    // Upload image data to the GPU
    SetTextureData(tex, pixel_data);
    return true;
}

bool RendererGL43::RegisterShader(ShaderResource* program,
                                std::string vertex_source, std::string pixel_source,
                                ShaderAttributeList inputs)
{
    ShaderResourceGL43* shader = static_cast<ShaderResourceGL43*>(program);

    // Initialize and compile the shaders
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    shader->shaders_.push_back(vertex_shader);
    GLuint pixel_shader = glCreateShader(GL_FRAGMENT_SHADER);
    shader->shaders_.push_back(pixel_shader);
    const char* vs = vertex_source.data();
    const char* ps = pixel_source.data();
    glShaderSource(vertex_shader, 1, &vs, nullptr);
    glShaderSource(pixel_shader, 1, &ps, nullptr);
    glCompileShader(vertex_shader);
    glCompileShader(pixel_shader);

    // Check that everything went OK
    int vert_result, pixel_result;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vert_result);
    if (!vert_result)
    {
        LogCompileErrors(vertex_shader, true);
        return false;
    }
    glGetShaderiv(pixel_shader, GL_COMPILE_STATUS, &pixel_result);
    if (!pixel_result)
    {
        LogCompileErrors(pixel_shader, true);
        return false;
    }

    // Take our shaders and turn it into a render pipeline
    shader->program_ = glCreateProgram();
    glAttachShader(shader->program_, vertex_shader);
    glAttachShader(shader->program_, pixel_shader);
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
    shader->type_ = ShaderResourceGL43::PIPELINE;

    return true;
}

bool RendererGL43::RegisterComputeShader(ShaderResource* program, std::string source)
{
    ShaderResourceGL43* shader = static_cast<ShaderResourceGL43*>(program);

    // Initialize and compile the shaders
    GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    shader->shaders_.push_back(compute_shader);
    const char* cs = source.data();
    glShaderSource(compute_shader, 1, &cs, nullptr);
    glCompileShader(compute_shader);

    // Check that everything went OK
    int shader_result;
    glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &shader_result);
    if (!shader_result)
    {
        LogCompileErrors(compute_shader, true);
        return false;
    }

    // Take our shaders and turn it into a render pipeline
    shader->program_ = glCreateProgram();
    glAttachShader(shader->program_, compute_shader);
    glLinkProgram(shader->program_);

    // Check that everything went OK
    int link_result;
    glGetProgramiv(shader->program_, GL_LINK_STATUS, &link_result);
    if (!link_result)
    {
        LogCompileErrors(shader->program_, false);
        return false;
    }
    shader->type_ = ShaderResourceGL43::COMPUTE;

    return true;
}

void RendererGL43::RenderShader(ShaderResource* program, unsigned int index_count)
{
    UnmapBuffers();

    ShaderResourceGL43* shader = static_cast<ShaderResourceGL43*>(program);

    if (shader->type_ != ShaderResourceGL43::PIPELINE)
    {
        throw "Bad shader type sent to rendering pipeline";
    }

    BindShader(shader->program_);

    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    // TODO: make this only called once per shader somehow
    // nvogl32.dll loves it when i clean up my VAOs!
    glBindVertexArray(0);
}

void RendererGL43::RunComputeShader(ShaderResource* program, unsigned int groups_x,
                                  unsigned int groups_y, unsigned int groups_z)
{
    ShaderResourceGL43* shader = static_cast<ShaderResourceGL43*>(program);

    if (shader->type_ != ShaderResourceGL43::COMPUTE)
    {
        throw "Bad shader type sent to computing pipeline";
    }

    BindShader(shader->program_);
    glDispatchCompute(groups_x, groups_y, groups_z);
}

void RendererGL43::BindFramebuffer(FramebufferResource* frame_buffer)
{
    if (frame_buffer != nullptr)
    {
        FramebufferResourceGL43* fbo = static_cast<FramebufferResourceGL43*>(frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->framebuffer_);
        glViewport(0, 0, fbo->width, fbo->height);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screen_.width, screen_.height);
    }
}

std::vector<const TextureResource*> RendererGL43::FramebufferTextures(FramebufferResource* frame_buffer)
{
    FramebufferResourceGL43* fbo = static_cast<FramebufferResourceGL43*>(frame_buffer);

    std::vector<const TextureResource*> targets;
    for (const auto& tex : fbo->targets_)
    {
        targets.push_back(tex.get());
    }

    return targets;
}

const TextureResource* RendererGL43::FramebufferDepthTexture(FramebufferResource* frame_buffer)
{
    FramebufferResourceGL43* fbo = static_cast<FramebufferResourceGL43*>(frame_buffer);

    return fbo->depth_.get();
}

void RendererGL43::BindMeshBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)
{
    UnmapBuffers();

    BufferResourceGL43* vertex_buf = static_cast<BufferResourceGL43*>(vertex_buffer);
    glBindVertexArray(vertex_buf->vertex_array_id_);
}

void RendererGL43::SetMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                             const Vertex* vertices, unsigned int vert_count,
                             const unsigned int* indices, unsigned int index_count)
{
    BufferResourceGL43* vertex_buf = static_cast<BufferResourceGL43*>(vertex_buffer);
    BufferResourceGL43* index_buf = static_cast<BufferResourceGL43*>(index_buffer);

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

void RendererGL43::UpdateMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                const Vertex* vertices, unsigned int vert_offset, unsigned int vert_count,
                                const unsigned int* indices, unsigned int index_offset, unsigned int index_count)
{
    BufferResourceGL43* vertex_buf = static_cast<BufferResourceGL43*>(vertex_buffer);
    BufferResourceGL43* index_buf  = static_cast<BufferResourceGL43*>(index_buffer);

    glBindVertexArray(vertex_buf->vertex_array_id_);
    // Attach vertex buffer data to VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glBufferSubData(GL_ARRAY_BUFFER, vert_offset * sizeof(Vertex), vert_count * sizeof(Vertex), vertices);

    // Attach index buffer data to VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf->buffer_);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset * sizeof(unsigned int), index_count * sizeof(unsigned int), indices);
}

void RendererGL43::MapMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                   Vertex** vertex_data, unsigned int** index_data)
{
    BufferResourceGL43* vertex_buf = static_cast<BufferResourceGL43*>(vertex_buffer);
    BufferResourceGL43* index_buf  = static_cast<BufferResourceGL43*>(index_buffer);

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

void RendererGL43::SetTextureData(TextureResource* texture, PixelData* pixels)
{
    auto tex = static_cast<TextureResourceGL43*>(texture);
    tex->type_ = GL_TEXTURE_2D;

    glBindTexture(tex->type_, tex->texture_);

    // Upload uncompressed textures manually because it allows us way more format options
    if (pixels->type.compression == TextureType::RAW)
    {
        GLint internal_format;
        GLint input_format;
        GLenum input_type;
        switch (pixels->type.format)
        {
        case TextureType::A8:
            internal_format = GL_R8;
            input_format = GL_RED;
            input_type = GL_UNSIGNED_BYTE;
            break;
        case TextureType::R8G8_UINT:
            internal_format = GL_RG8UI;
            input_format = GL_RG_INTEGER;
            input_type = GL_UNSIGNED_BYTE;
            break;
        case TextureType::R8G8B8_UINT:
            internal_format = GL_RGB8UI;
            input_format = GL_RGB_INTEGER;
            input_type = GL_UNSIGNED_BYTE;
            break;
        case TextureType::R8G8B8A8_UINT:
            internal_format = GL_RGBA8UI;
            input_format = GL_RGBA_INTEGER;
            input_type = GL_UNSIGNED_BYTE;
            break;
        case TextureType::R8G8B8A8:
            internal_format = GL_RGBA8;
            input_format = GL_RGBA;
            input_type = GL_UNSIGNED_BYTE;
            break;
        case TextureType::R16G16:
            internal_format = GL_RG16;
            input_format = GL_RG;
            input_type = GL_FLOAT;
            break;
        case TextureType::R32G32B32:
            internal_format = GL_RGB32F;
            input_format = GL_RGB;
            input_type = GL_FLOAT;
            break;
        case TextureType::R32G32B32A32:
            internal_format = GL_RGBA32F;
            input_format = GL_RGBA;
            input_type = GL_FLOAT;
            break;
        case TextureType::DEPTH:
            internal_format = GL_DEPTH_COMPONENT24;
            input_format = GL_DEPTH_COMPONENT;
            input_type = GL_FLOAT;
            break;
        case TextureType::NONE:
        case TextureType::R8G8B8:
        default:
            internal_format = GL_RGB8;
            input_format = GL_RGB;
            input_type = GL_UNSIGNED_BYTE;
            break;
        }
        glTexImage2D(tex->type_, 0, internal_format, pixels->width, pixels->height, 0, input_format, input_type, pixels->pixels.get());
    }
    // Upload compressed textures thru SOIL because its way easier
    else
    {
        unsigned int soil_flags = SOIL_FLAG_TEXTURE_REPEATS;
        if (pixels->type.compression == TextureType::DDS)
        {
            soil_flags |= SOIL_FLAG_DDS_LOAD_DIRECT;
        }
        else if (pixels->type.compression == TextureType::AUTO)
        {
            soil_flags |= SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_GL_MIPMAPS;
        }
        int channels = 0;
        switch (pixels->type.format)
        {
        case TextureType::A8:
            channels = 1;
            break;
        case TextureType::R8G8B8:
            channels = 3;
            break;
        case TextureType::R8G8B8A8:
            channels = 4;
            break;
        default:
            throw "Compressed texture using wrong format";
            break;
        }
        tex->texture_ = SOIL_create_OGL_texture(pixels->pixels.get(), &pixels->width, &pixels->height,
            channels, tex->texture_, soil_flags);
        if (tex->texture_ == 0)
        {
            throw "Failed to update compressed texture";
        }
    }

    // Apply our texture settings (we do this after to override SOIL settings)
    if (pixels->type.wrap == TextureType::CLAMP)
    {
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    if (pixels->type.filter == TextureType::NEAREST)
    {
        glTexParameteri(tex->type_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if (pixels->type.compression == TextureType::AUTO)
        {
            glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        }
        else
        {
            glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
    }
    else
    {
        glTexParameteri(tex->type_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (pixels->type.compression == TextureType::AUTO)
        {
            glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
    }

    // TODO: attach this to a setting + safety check for max
    // glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &float);
    glTexParameteri(tex->type_, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

    // TODO: Re enable this for non-dds textures
    // glGenerateMipmap(tex->type_);
}

void RendererGL43::SetTextureData(TextureResource* texture, PixelData3D* pixels)
{
    auto tex = static_cast<TextureResourceGL43*>(texture);
    tex->type_ = GL_TEXTURE_3D;

    glBindTexture(tex->type_, tex->texture_);

    GLint internal_format;
    GLint input_format;
    GLenum input_type;
    switch (pixels->type.format)
    {
    case TextureType::A8:
        internal_format = GL_R8;
        input_format = GL_RED;
        input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R8G8_UINT:
        internal_format = GL_RG8UI;
        input_format = GL_RG_INTEGER;
        input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R8G8B8_UINT:
        internal_format = GL_RGB8UI;
        input_format = GL_RGB_INTEGER;
        input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R8G8B8A8_UINT:
        internal_format = GL_RGBA8UI;
        input_format = GL_RGBA_INTEGER;
        input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R8G8B8A8:
        internal_format = GL_RGBA8;
        input_format = GL_RGBA;
        input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R16G16:
        internal_format = GL_RG16;
        input_format = GL_RG;
        input_type = GL_FLOAT;
        break;
    case TextureType::R32G32B32:
        internal_format = GL_RGB32F;
        input_format = GL_RGB;
        input_type = GL_FLOAT;
        break;
    case TextureType::R32G32B32A32:
        internal_format = GL_RGBA32F;
        input_format = GL_RGBA;
        input_type = GL_FLOAT;
        break;
    case TextureType::DEPTH:
        internal_format = GL_DEPTH_COMPONENT24;
        input_format = GL_DEPTH_COMPONENT;
        input_type = GL_FLOAT;
        break;
    case TextureType::NONE:
    case TextureType::R8G8B8:
    default:
        internal_format = GL_RGB8;
        input_format = GL_RGB;
        input_type = GL_UNSIGNED_BYTE;
        break;
    }
    glTexImage3D(tex->type_, 0, internal_format, pixels->width, pixels->height, pixels->depth, 0, input_format, input_type, pixels->pixels.get());

    // Apply our texture settings (we do this after to override SOIL settings)
    if (pixels->type.wrap == TextureType::CLAMP)
    {
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    else
    {
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(tex->type_, GL_TEXTURE_WRAP_R, GL_REPEAT);
    }
    if (pixels->type.filter == TextureType::NEAREST)
    {
        glTexParameteri(tex->type_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else
    {
        glTexParameteri(tex->type_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const float value)
{
    auto prog = static_cast<ShaderResourceGL43*>(program);
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const int value)
{
    auto prog = static_cast<ShaderResourceGL43*>(program);
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Matrix value)
{
    auto prog = static_cast<ShaderResourceGL43*>(program);
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Vector2 value)
{
    auto prog = static_cast<ShaderResourceGL43*>(program);
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Vector3 value)
{
    auto prog = static_cast<ShaderResourceGL43*>(program);
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Vector4 value)
{
    auto prog = static_cast<ShaderResourceGL43*>(program);
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const TextureResource* value, unsigned int texture_index)
{
    const TextureResourceGL43* tex = static_cast<const TextureResourceGL43*>(value);
    glActiveTexture(GL_TEXTURE0 + texture_index);
    glBindTexture(tex->type_, tex->texture_);
    return SetShaderInput(program, name, static_cast<int>(texture_index));
}

bool RendererGL43::SetBlendMode(BlendMode mode)
{
    GLint scfunc, dcfunc, safunc, dafunc;
    switch (mode)
    {
    case ADDITIVE:
        scfunc = GL_ONE;
        dcfunc = GL_ONE;
        safunc = GL_ONE;
        dafunc = GL_ONE;
        break;
    default:
    case ALPHA:
        scfunc = GL_SRC_ALPHA;
        dcfunc = GL_ONE_MINUS_SRC_ALPHA;
        safunc = GL_ONE;
        dafunc = GL_ONE_MINUS_SRC_ALPHA;
        break;
    case OVERWRITE:
        scfunc = GL_ONE;
        dcfunc = GL_ZERO;
        safunc = GL_ONE;
        dafunc = GL_ZERO;
    }
    glBlendFuncSeparate(scfunc, dcfunc, safunc, dafunc);

    return true;
}

bool RendererGL43::SetCullMode(CullMode mode)
{
    switch (mode)
    {
    case DISABLE:
        glDisable(GL_CULL_FACE);
        break;
    case ENABLE_CW:
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        break;
    case ENABLE_CCW:
    default:
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        break;
    }

    return true;
}

bool RendererGL43::SetDepthTesting(bool enable)
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

bool RendererGL43::SetViewport(Box viewport)
{
    glViewport(static_cast<GLint>(viewport.x),
               static_cast<GLint>(viewport.y),
               static_cast<GLint>(viewport.w),
               static_cast<GLint>(viewport.h));
    return true;
}

void RendererGL43::VideoCardInfo(char* name, int& memory)
{
    strcpy_s(name, 128, video_card_desc_.c_str());
    memory = video_card_memory_;
    return;
}

bool RendererGL43::LoadPixelData(std::string filename, PixelData* data)
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
        data->type.compression = TextureType::DDS;
    }
    else
    {
        data->type.compression = TextureType::AUTO;
    }

    switch (channels)
    {
    case 1:
        data->type.format = TextureType::A8;
        break;
    case 3:
        data->type.format = TextureType::R8G8B8;
        break;
    case 4:
        data->type.format = TextureType::R8G8B8A8;
        break;
    default:
        data->type.format = TextureType::R8G8B8A8;
        break;
    }
    return true;
}

void RendererGL43::BindShader(GLuint shader)
{
    // Avoid repeated calls to glUseProgram (perf boost)
    if (shader != active_shader_)
    {
        glUseProgram(shader);
    }
    active_shader_ = shader;
}

void RendererGL43::UnbindShader()
{
    active_shader_ = 0;
}

void RendererGL43::UnmapBuffers()
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

void RendererGL43::LogCompileErrors(GLuint resource, bool is_shader)
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
