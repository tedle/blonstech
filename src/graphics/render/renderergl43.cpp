////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
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
// Safe type casting to prevent using resources in the wrong context
template<typename T, typename U>
T resource_cast(U value, Renderer::ContextID current_id)
{
    if (value->context_id != current_id)
    {
        throw "Renderering context mismatch";
    }
    return static_cast<T>(value);
}
// Converts engine axis into OpenGL cube map axis enum
GLenum TranslateAxisFormat(AxisAlignedNormal axis)
{
    switch (axis)
    {
    case AxisAlignedNormal::POSITIVE_X: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    case AxisAlignedNormal::NEGATIVE_X: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    case AxisAlignedNormal::POSITIVE_Y: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    case AxisAlignedNormal::NEGATIVE_Y: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    case AxisAlignedNormal::POSITIVE_Z: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    case AxisAlignedNormal::NEGATIVE_Z: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    default:
        throw "Unsupported axis in translation layer";
        break;
    }
}
// Converts PixelData type into OpenGL texture enum
template <typename T>
constexpr GLenum TranslatePixelDataType()
{
    // TODO: Make this a constexpr-if when we have C++17
    return (
        std::is_same<T, PixelData>::value ? GL_TEXTURE_2D :
        std::is_same<T, PixelData3D>::value ? GL_TEXTURE_3D :
        std::is_same<T, PixelDataCubemap>::value ? GL_TEXTURE_CUBE_MAP :
        GL_FALSE
    );
}
// Converts engine texture type into OpenGL texture type
void TranslateTextureFormat(TextureType::Format format, GLint* internal_format, GLint* input_format, GLenum* input_type)
{
    switch (format)
    {
    case TextureType::A8:
        *internal_format = GL_R8;
        *input_format = GL_RED;
        *input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R8G8_UINT:
        *internal_format = GL_RG8UI;
        *input_format = GL_RG_INTEGER;
        *input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R8G8B8_UINT:
        *internal_format = GL_RGB8UI;
        *input_format = GL_RGB_INTEGER;
        *input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R8G8B8A8_UINT:
        *internal_format = GL_RGBA8UI;
        *input_format = GL_RGBA_INTEGER;
        *input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R8G8B8A8:
        *internal_format = GL_RGBA8;
        *input_format = GL_RGBA;
        *input_type = GL_UNSIGNED_BYTE;
        break;
    case TextureType::R16G16:
        *internal_format = GL_RG16;
        *input_format = GL_RG;
        *input_type = GL_FLOAT;
        break;
    case TextureType::R16G16B16:
        *internal_format = GL_RGB16;
        *input_format = GL_RGB;
        *input_type = GL_FLOAT;
        break;
    case TextureType::R16G16B16A16:
        *internal_format = GL_RGBA16;
        *input_format = GL_RGBA;
        *input_type = GL_FLOAT;
        break;
    case TextureType::A32:
        *internal_format = GL_R32F;
        *input_format = GL_RED;
        *input_type = GL_FLOAT;
        break;
    case TextureType::R32G32:
        *internal_format = GL_RG32F;
        *input_format = GL_RG;
        *input_type = GL_FLOAT;
        break;
    case TextureType::R32G32B32:
        *internal_format = GL_RGB32F;
        *input_format = GL_RGB;
        *input_type = GL_FLOAT;
        break;
    case TextureType::R32G32B32A32:
        *internal_format = GL_RGBA32F;
        *input_format = GL_RGBA;
        *input_type = GL_FLOAT;
        break;
    case TextureType::DEPTH:
        *internal_format = GL_DEPTH_COMPONENT24;
        *input_format = GL_DEPTH_COMPONENT;
        *input_type = GL_FLOAT;
        break;
    case TextureType::NONE:
    case TextureType::R8G8B8:
    default:
        *internal_format = GL_RGB8;
        *input_format = GL_RGB;
        *input_type = GL_UNSIGNED_BYTE;
        break;
    }
}
// Overloaded glUniforms to keep things generic
void Uniform(GLuint loc, const float* value, GLsizei elements)
{
    glUniform1fv(loc, elements, value);
}
void Uniform(GLuint loc, const int* value, GLsizei elements)
{
    glUniform1iv(loc, elements, value);
}
void Uniform(GLuint loc, const Matrix* value, GLsizei elements)
{
    glUniformMatrix4fv(loc, elements, GL_FALSE, (float*)value->m);
}
void Uniform(GLuint loc, const Vector2* value, GLsizei elements)
{
    glUniform2fv(loc, elements, &value->x);
}
void Uniform(GLuint loc, const Vector3* value, GLsizei elements)
{
    glUniform3fv(loc, elements, &value->x);
}
void Uniform(GLuint loc, const Vector4* value, GLsizei elements)
{
    glUniform4fv(loc, elements, &value->x);
}
} // namespace

class BufferResourceGL43 : public BufferResource
{
public:
    BufferResourceGL43(Renderer::ContextID parent_id) : BufferResource(parent_id) {}
    ~BufferResourceGL43() override;

    GLuint buffer_, vertex_array_id_;
    enum BufferType { VERTEX_BUFFER, INDEX_BUFFER } type_;
    DrawMode draw_mode_;
};

class TextureResourceGL43 : public TextureResource
{
public:
    TextureResourceGL43(Renderer::ContextID parent_id) : TextureResource(parent_id) {}
    ~TextureResourceGL43() override;

    GLuint texture_;
    GLint type_; ///< GL_TEXTURE_2D or GL_TEXTURE_3D
    TextureType options_;
    bool has_mipmaps_;
};

class FramebufferResourceGL43 : public FramebufferResource
{
public:
    FramebufferResourceGL43(Renderer::ContextID parent_id) : FramebufferResource(parent_id) {}
    ~FramebufferResourceGL43() override;

    GLuint framebuffer_;
    units::pixel width, height;
    std::vector<std::unique_ptr<TextureResourceGL43>> targets_;
    std::unique_ptr<TextureResourceGL43> depth_;
};

class ShaderDataResourceGL43 : public ShaderDataResource
{
public:
    ShaderDataResourceGL43(Renderer::ContextID parent_id) : ShaderDataResource(parent_id) {}
    ~ShaderDataResourceGL43() override;

    GLuint buffer_;
    std::size_t size_;
};

class ShaderResourceGL43 : public ShaderResource
{
public:
    ShaderResourceGL43(Renderer::ContextID parent_id) : ShaderResource(parent_id) {}
    ~ShaderResourceGL43() override;

    GLuint program_;
    std::vector<GLuint> shaders_;
    enum ShaderType { NONE, PIPELINE, COMPUTE } type_;

    GLint UniformLocation(const char* name);
    template <typename T>
    bool SetUniform(const char* name, T* value, GLsizei elements);
    template <typename T>
    bool SetUniform(const char* name, T value);
    bool BindSSBO(const char* name, const ShaderDataResourceGL43* ssbo);

private:
    struct HashFunc { unsigned int operator()(const char* s) const { return FastHash(s); } };
    struct CompFunc { bool operator()(const char* a, const char* b) const { return strcmp(a, b) == 0; } };
    std::unordered_map<const char*, GLint, HashFunc, CompFunc> uniform_location_cache_;
    std::unordered_map<const char*, GLint, HashFunc, CompFunc> ssbo_binding_point_cache_;
    // slow, but clean. possible memory leak with const char* key anyway, iduno
    // std::unordered_map<std::string, GLint> uniform_location_cache_;
};

BufferResourceGL43::~BufferResourceGL43()
{
    auto active_context = render::context();
    // TODO: Something cleaner in case owning context isnt actually deleted (inactive?)
    // This is save when deleted since OpenGL contexts clean up after themselves
    if (context_id != active_context->id())
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
    if (context_id != active_context->id())
    {
        return;
    }

    active_context->BindFramebuffer(nullptr);
    glDeleteFramebuffers(1, &framebuffer_);
}

TextureResourceGL43::~TextureResourceGL43()
{
    auto active_context = render::context();
    if (context_id != active_context->id())
    {
        return;
    }

    glDeleteTextures(1, &texture_);
}

ShaderResourceGL43::~ShaderResourceGL43()
{
    auto active_context = render::context();
    if (context_id != active_context->id())
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

ShaderDataResourceGL43::~ShaderDataResourceGL43()
{
    auto active_context = render::context();
    if (context_id != active_context->id())
    {
        return;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glDeleteBuffers(1, &buffer_);
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
bool ShaderResourceGL43::SetUniform(const char* name, T* value, GLsizei elements)
{
    auto context = static_cast<RendererGL43*>(render::context());
    context->BindShader(program_);

    auto location = UniformLocation(name);
    if (location < 0)
    {
        return false;
    }
    Uniform(location, value, elements);
    return true;
}

template <typename T>
bool ShaderResourceGL43::SetUniform(const char* name, T value)
{
    return SetUniform(name, &value, 1);
}

bool ShaderResourceGL43::BindSSBO(const char* name, const ShaderDataResourceGL43* ssbo)
{
    auto context = static_cast<RendererGL43*>(render::context());
    context->BindShader(program_);

    GLint binding_point = 0;
    auto it = ssbo_binding_point_cache_.find(name);
    if (it == ssbo_binding_point_cache_.end())
    {
        auto index = glGetProgramResourceIndex(program_, GL_SHADER_STORAGE_BLOCK, name);
        if (index == GL_INVALID_INDEX)
        {
            return false;
        }
        binding_point = static_cast<GLint>(ssbo_binding_point_cache_.size());
        glShaderStorageBlockBinding(program_, index, binding_point);
        ssbo_binding_point_cache_[_strdup(name)] = binding_point;
    }
    else
    {
        binding_point = it->second;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, ssbo->buffer_);
    return true;
}

RendererGL43::RendererGL43(Client::Info screen_info, bool vsync, bool fullscreen)
{
    // Mitigates repeated calls to glUseProgram
    active_shader_ = 0;
    // Mitigates repeated calls to glBindFramebuffer
    active_framebuffer_ = 0;

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

    // Initialize debug output
    InitializeDebugOutput();

    // Grab video card info
    video_card_desc_ = (char*)glGetString(GL_VENDOR);
    video_card_desc_ += " ";
    video_card_desc_ += (char*)glGetString(GL_RENDERER);
    // This isn't supported in OpenGL
    video_card_memory_ = 0;

    // Enable depth testing, with a default of 1.0
    glClearDepth(1.0);
    SetDepthTesting(true);

    // Configure backface culling
    SetCullMode(ENABLE_CCW);

    // Enable transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the row padding on textures to be 1 (default 4)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

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

ShaderDataResource* RendererGL43::MakeShaderDataResource()
{
    return new ShaderDataResourceGL43(id());
}

bool RendererGL43::RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                                Vertex* vertices, unsigned int vert_count,
                                unsigned int* indices, unsigned int index_count,
                                DrawMode draw_mode)
{
    BufferResourceGL43* vertex_buf = resource_cast<BufferResourceGL43*>(vertex_buffer, id());
    BufferResourceGL43* index_buf = resource_cast<BufferResourceGL43*>(index_buffer, id());

    // Set the buffer types
    vertex_buf->type_ = BufferResourceGL43::VERTEX_BUFFER;
    index_buf->type_ = BufferResourceGL43::INDEX_BUFFER;

    // Set the draw mode
    vertex_buf->draw_mode_ = draw_mode;
    index_buf->draw_mode_ = draw_mode;

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
    glVertexAttribPointer(POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // UV declaration
    glVertexAttribPointer(TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
    // Lightmap UV declaration
    glVertexAttribPointer(LIGHT_TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5*sizeof(float)));
    // Normal declaration
    glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7*sizeof(float)));
    // Tangent declaration
    glVertexAttribPointer(TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(10*sizeof(float)));
    // Bitangent declaration
    glVertexAttribPointer(BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(13*sizeof(float)));

    // Setup the index buffer
    glGenBuffers(1, &index_buf->buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf->buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // nvogl32.dll loves it when i clean up my VAOs!
    glBindVertexArray(0);

    return true;
}

bool RendererGL43::RegisterFramebuffer(FramebufferResource* frame_buffer,
                                     units::pixel width, units::pixel height,
                                     std::vector<TextureType> formats, bool store_depth)
{
    FramebufferResourceGL43* fbo = resource_cast<FramebufferResourceGL43*>(frame_buffer, id());

    fbo->width = width;
    fbo->height = height;

    // Create the frame buffer
    glGenFramebuffers(1, &fbo->framebuffer_);

    // Bind it so we can put stuff in it
    BindFramebuffer(fbo);

    // Creates empty render targets
    auto make_texture = [&](TextureType type)
    {
        auto tex = std::make_unique<TextureResourceGL43>(id());
        PixelData pixels;
        pixels.type = type;
        pixels.width = width;
        pixels.height = height;
        RegisterTexture(tex.get(), &pixels);

        return tex;
    };

    // Create and bind all of our render targets
    std::vector<const TextureResource*> colour_targets;
    for (unsigned int i = 0; i < formats.size(); i++)
    {
        if (formats[i].format != TextureType::NONE)
        {
            fbo->targets_.push_back(make_texture(formats[i]));
            colour_targets.push_back(fbo->targets_.back().get());
        }
    }
    SetFramebufferColourTextures(fbo, colour_targets, 0);

    if (store_depth)
    {
        // Create and bind the depth target
        fbo->depth_ = make_texture({ TextureType::DEPTH, TextureType::LINEAR, TextureType::CLAMP });
        SetFramebufferDepthTexture(fbo, fbo->depth_.get(), 0);
    }

    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

namespace
{
template <typename T, GLenum texture_type = TranslatePixelDataType<T>()>
bool RegisterTextureTemplate(TextureResource* texture, T* pixel_data, RendererGL43* context)
{
    static_assert(texture_type != GL_FALSE, "Unsupported PixelData type for texture registration");

    TextureResourceGL43* tex = resource_cast<TextureResourceGL43*>(texture, context->id());
    tex->type_ = texture_type;
    tex->options_ = pixel_data->type;
    tex->has_mipmaps_ = false;

    // Generate an ID for the texture.
    glGenTextures(1, &tex->texture_);

    // Bind the texture based on the statically determined type
    glBindTexture(tex->type_, tex->texture_);

    // Upload image data to the GPU
    context->SetTextureData(tex, pixel_data, 0);
    return true;
}
} // namespace

bool RendererGL43::RegisterTexture(TextureResource* texture, PixelData* pixel_data)
{
    return RegisterTextureTemplate(texture, pixel_data, this);
}

bool RendererGL43::RegisterTexture(TextureResource* texture, PixelData3D* pixel_data)
{
    return RegisterTextureTemplate(texture, pixel_data, this);
}

bool RendererGL43::RegisterTexture(TextureResource* texture, PixelDataCubemap* pixel_data)
{
    return RegisterTextureTemplate(texture, pixel_data, this);
}

bool RendererGL43::RegisterShader(ShaderResource* program,
                                std::string vertex_source, std::string pixel_source,
                                ShaderAttributeList inputs)
{
    ShaderResourceGL43* shader = resource_cast<ShaderResourceGL43*>(program, id());

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
    ShaderResourceGL43* shader = resource_cast<ShaderResourceGL43*>(program, id());

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

void RendererGL43::RegisterShaderData(ShaderDataResource* data_handle, const void* data, std::size_t size)
{
    auto data_buffer = resource_cast<ShaderDataResourceGL43*>(data_handle, id());
    glGenBuffers(1, &data_buffer->buffer_);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, data_buffer->buffer_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
    data_buffer->size_ = size;
}

void RendererGL43::RenderShader(ShaderResource* program, unsigned int index_count)
{
    RenderShaderInstanced(program, index_count, 1);
}

void RendererGL43::RenderShaderInstanced(ShaderResource* program, unsigned int index_count, unsigned int instance_count)
{
    UnmapBuffers();

    ShaderResourceGL43* shader = resource_cast<ShaderResourceGL43*>(program, id());

    if (shader->type_ != ShaderResourceGL43::PIPELINE)
    {
        throw "Bad shader type sent to rendering pipeline";
    }

    BindShader(shader->program_);

    if (instance_count > 1)
    {
        glDrawElementsInstanced(draw_mode_, index_count, GL_UNSIGNED_INT, 0, instance_count);
    }
    else
    {
        glDrawElements(draw_mode_, index_count, GL_UNSIGNED_INT, 0);
    }
    // TODO: Add some kind of option for enabling this? It's necessary if we want to
    // write to incoherent memory in a pipeline shader but also slows things significantly.
    // Would probably not be noticable if restricted to only deferred rendering calls
    // and ignored during g-buffer rendering:
    //     glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // TODO: make this only called once per shader somehow
    // nvogl32.dll loves it when i clean up my VAOs!
    glBindVertexArray(0);
}

void RendererGL43::RunComputeShader(ShaderResource* program, unsigned int groups_x,
                                    unsigned int groups_y, unsigned int groups_z)
{
    ShaderResourceGL43* shader = resource_cast<ShaderResourceGL43*>(program, id());

    if (shader->type_ != ShaderResourceGL43::COMPUTE)
    {
        throw "Bad shader type sent to computing pipeline";
    }

    BindShader(shader->program_);
    glDispatchCompute(groups_x, groups_y, groups_z);
    // TODO: Test performance implications of aggressive mem barrier once we have more stuff to test it on
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void RendererGL43::BindFramebuffer(FramebufferResource* frame_buffer)
{
    if (frame_buffer != nullptr)
    {
        FramebufferResourceGL43* fbo = resource_cast<FramebufferResourceGL43*>(frame_buffer, id());
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->framebuffer_);
        active_framebuffer_ = fbo->framebuffer_;
        glViewport(0, 0, fbo->width, fbo->height);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        active_framebuffer_ = 0;
        glViewport(0, 0, screen_.width, screen_.height);
    }
}

void RendererGL43::SetFramebufferColourTextures(FramebufferResource* frame_buffer, const std::vector<const TextureResource*>& colour_textures, unsigned int mip_level)
{
    BindFramebuffer(frame_buffer);
    std::unique_ptr<GLenum[]> drawbuffers(new GLenum[colour_textures.size()]);
    for (unsigned int i = 0; i < colour_textures.size(); i++)
    {
        auto attachment = GL_COLOR_ATTACHMENT0 + i;
        auto tex = resource_cast<const TextureResourceGL43*>(colour_textures[i], id());
        glFramebufferTexture(GL_FRAMEBUFFER, attachment, tex->texture_, mip_level);
        drawbuffers[i] = attachment;
    }
    glDrawBuffers(static_cast<GLsizei>(colour_textures.size()), drawbuffers.get());
}

void RendererGL43::SetFramebufferDepthTexture(FramebufferResource* frame_buffer, const TextureResource* depth_texture, unsigned int mip_level)
{
    if (frame_buffer == nullptr)
    {
        throw "Framebuffer cannot be null";
    }
    FramebufferResourceGL43* fbo = resource_cast<FramebufferResourceGL43*>(frame_buffer, id());

    // If depth_texture is a nullptr, unbind the depth attachment
    GLuint depth = 0;
    if (depth_texture != nullptr)
    {
        const TextureResourceGL43* tex = resource_cast<const TextureResourceGL43*>(depth_texture, id());
        depth = tex->texture_;
    }
    BindFramebuffer(frame_buffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, mip_level);
}

std::vector<const TextureResource*> RendererGL43::FramebufferTextures(FramebufferResource* frame_buffer)
{
    FramebufferResourceGL43* fbo = resource_cast<FramebufferResourceGL43*>(frame_buffer, id());

    std::vector<const TextureResource*> targets;
    for (const auto& tex : fbo->targets_)
    {
        targets.push_back(tex.get());
    }

    return targets;
}

const TextureResource* RendererGL43::FramebufferDepthTexture(FramebufferResource* frame_buffer)
{
    FramebufferResourceGL43* fbo = resource_cast<FramebufferResourceGL43*>(frame_buffer, id());

    return fbo->depth_.get();
}

// TODO: Change this to take VAO or some representation thereof instead? Or at least remove need for index buffer
void RendererGL43::BindMeshBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)
{
    UnmapBuffers();

    BufferResourceGL43* vertex_buf = resource_cast<BufferResourceGL43*>(vertex_buffer, id());
    glBindVertexArray(vertex_buf->vertex_array_id_);

    switch (vertex_buf->draw_mode_)
    {
    case LINES:
        draw_mode_ = GL_LINES;
        break;
    case TRIANGLES:
        draw_mode_ = GL_TRIANGLES;
        break;
    default:
        throw "Unknown draw mode set for mesh buffer";
        break;
    }
}

void RendererGL43::SetMeshData(BufferResource* vertex_buffer, BufferResource* index_buffer,
                             const Vertex* vertices, unsigned int vert_count,
                             const unsigned int* indices, unsigned int index_count)
{
    BufferResourceGL43* vertex_buf = resource_cast<BufferResourceGL43*>(vertex_buffer, id());
    BufferResourceGL43* index_buf = resource_cast<BufferResourceGL43*>(index_buffer, id());

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
    BufferResourceGL43* vertex_buf = resource_cast<BufferResourceGL43*>(vertex_buffer, id());
    BufferResourceGL43* index_buf  = resource_cast<BufferResourceGL43*>(index_buffer, id());

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
    BufferResourceGL43* vertex_buf = resource_cast<BufferResourceGL43*>(vertex_buffer, id());
    BufferResourceGL43* index_buf  = resource_cast<BufferResourceGL43*>(index_buffer, id());

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

void RendererGL43::SetTextureData(TextureResource* texture, PixelData* pixels, unsigned int mip_level)
{
    auto tex = resource_cast<TextureResourceGL43*>(texture, id());
    if (mip_level != 0)
    {
        if (pixels->type.compression == TextureType::DDS)
        {
            throw "Mipmap level support in 2D textures is not supported for TextureType::DDS";
        }
        tex->has_mipmaps_ = true;
    }
    tex->type_ = TranslatePixelDataType<PixelData>();
    tex->options_ = pixels->type;

    glBindTexture(tex->type_, tex->texture_);

    // Upload uncompressed textures manually because it allows us way more format options
    if (pixels->type.compression != TextureType::DDS)
    {
        GLint internal_format;
        GLint input_format;
        GLenum input_type;
        TranslateTextureFormat(pixels->type.format, &internal_format, &input_format, &input_type);
        glTexImage2D(tex->type_, mip_level, internal_format, pixels->width, pixels->height, 0, input_format, input_type, pixels->pixels.data());
        if (pixels->type.compression == TextureType::AUTO && mip_level == 0)
        {
            glGenerateMipmap(tex->type_);
            tex->has_mipmaps_ = true;
        }
    }
    // Upload compressed textures thru SOIL because its way easier
    else
    {
        unsigned int soil_flags = SOIL_FLAG_TEXTURE_REPEATS;
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
        soil_flags |= SOIL_FLAG_DDS_LOAD_DIRECT;
        tex->texture_ = SOIL_direct_load_DDS_from_memory(pixels->pixels.data(), static_cast<int>(pixels->pixels.size()), tex->texture_, soil_flags, 0);

        // Ugly header parsing, mipmap count starts at 24th byte of header, header starts after 4 byte magic number
        // https://msdn.microsoft.com/en-us/library/windows/desktop/bb943991(v=vs.85).aspx
        unsigned int mipmap_count = *reinterpret_cast<unsigned int*>(&pixels->pixels[4+24]);
        tex->has_mipmaps_ = mipmap_count > 0;

        if (tex->texture_ == 0)
        {
            throw "Failed to upload compressed texture";
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
        if (tex->has_mipmaps_)
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
        if (tex->has_mipmaps_)
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
}

void RendererGL43::SetTextureData(TextureResource* texture, PixelData3D* pixels, unsigned int mip_level)
{
    auto tex = resource_cast<TextureResourceGL43*>(texture, id());
    if (tex->options_.compression == TextureType::DDS)
    {
        throw "TextureType::DDS is not supported for 3D textures";
    }
    if (mip_level != 0)
    {
        tex->has_mipmaps_ = true;
    }
    tex->type_ = TranslatePixelDataType<PixelData3D>();
    tex->options_ = pixels->type;

    glBindTexture(tex->type_, tex->texture_);

    GLint internal_format;
    GLint input_format;
    GLenum input_type;
    TranslateTextureFormat(pixels->type.format, &internal_format, &input_format, &input_type);
    glTexImage3D(tex->type_, mip_level, internal_format, pixels->width, pixels->height, pixels->depth, 0, input_format, input_type, pixels->pixels.data());
    if (pixels->type.compression == TextureType::AUTO && mip_level == 0)
    {
        glGenerateMipmap(tex->type_);
        tex->has_mipmaps_ = true;
    }

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
        if (tex->has_mipmaps_)
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
        if (tex->has_mipmaps_)
        {
            glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
    }
}

void RendererGL43::SetTextureData(TextureResource* texture, PixelDataCubemap* pixels, unsigned int mip_level)
{
    auto tex = resource_cast<TextureResourceGL43*>(texture, id());
    if (tex->options_.compression != TextureType::RAW)
    {
        throw "TextureType::DDS is not supported for cubemap textures";
    }
    if (mip_level != 0)
    {
        tex->has_mipmaps_ = true;
    }
    tex->type_ = TranslatePixelDataType<PixelDataCubemap>();
    tex->options_ = pixels->type;

    glBindTexture(tex->type_, tex->texture_);

    GLint internal_format;
    GLint input_format;
    GLenum input_type;
    TranslateTextureFormat(pixels->type.format, &internal_format, &input_format, &input_type);
    for (const auto& axis : { POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z })
    {
        auto cubeface = TranslateAxisFormat(axis);
        glTexImage2D(cubeface, mip_level, internal_format, pixels->width, pixels->height, 0, input_format, input_type, pixels->pixels[axis].data());
    }
    if (pixels->type.compression == TextureType::AUTO && mip_level == 0)
    {
        glGenerateMipmap(tex->type_);
        tex->has_mipmaps_ = true;
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
        if (tex->has_mipmaps_)
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
        if (tex->has_mipmaps_)
        {
            glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            glTexParameteri(tex->type_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
    }
}

PixelData RendererGL43::GetTextureData(const TextureResource* texture, unsigned int mip_level)
{
    auto tex = resource_cast<const TextureResourceGL43*>(texture, id());
    if (tex->type_ != TranslatePixelDataType<PixelData>())
    {
        throw "Attemped to retrieve mismatched texture type";
    }
    if (tex->options_.compression == TextureType::DDS)
    {
        throw "Attemped to retrieve compressed texture type";
    }
    if (mip_level != 0 && tex->has_mipmaps_ == false)
    {
        throw "Attempted to retrieve mipmap of single level texture";
    }
    GLint width, height;
    GLint internal_format, input_format;
    GLenum input_type;
    glBindTexture(tex->type_, tex->texture_);
    glGetTexLevelParameteriv(tex->type_, mip_level, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(tex->type_, mip_level, GL_TEXTURE_HEIGHT, &height);
    TranslateTextureFormat(tex->options_.format, &internal_format, &input_format, &input_type);

    PixelData pixels;
    pixels.width = width;
    pixels.height = height;
    pixels.type = tex->options_;
    pixels.pixels.resize(width * height * (pixels.bits_per_pixel() / 8));
    glGetTexImage(tex->type_, mip_level, input_format, input_type, pixels.pixels.data());
    return pixels;
}

PixelData3D RendererGL43::GetTextureData3D(const TextureResource* texture, unsigned int mip_level)
{
    auto tex = resource_cast<const TextureResourceGL43*>(texture, id());
    if (tex->type_ != TranslatePixelDataType<PixelData3D>())
    {
        throw "Attemped to retrieve mismatched texture type";
    }
    if (tex->options_.compression == TextureType::DDS)
    {
        throw "Attemped to retrieve compressed texture type";
    }
    if (mip_level != 0 && tex->has_mipmaps_ == false)
    {
        throw "Attempted to retrieve mipmap of single level texture";
    }
    GLint width, height, depth;
    GLint internal_format, input_format;
    GLenum input_type;
    glBindTexture(tex->type_, tex->texture_);
    glGetTexLevelParameteriv(tex->type_, mip_level, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(tex->type_, mip_level, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(tex->type_, mip_level, GL_TEXTURE_DEPTH, &depth);
    TranslateTextureFormat(tex->options_.format, &internal_format, &input_format, &input_type);

    PixelData3D pixels;
    pixels.width = width;
    pixels.height = height;
    pixels.depth = depth;
    pixels.type = tex->options_;
    pixels.pixels.resize(width * height * depth * (pixels.bits_per_pixel() / 8));
    glGetTexImage(tex->type_, mip_level, input_format, input_type, pixels.pixels.data());
    return pixels;
}

PixelDataCubemap RendererGL43::GetTextureDataCubemap(const TextureResource* texture, unsigned int mip_level)
{
    auto tex = resource_cast<const TextureResourceGL43*>(texture, id());
    if (tex->type_ != TranslatePixelDataType<PixelDataCubemap>())
    {
        throw "Attemped to retrieve mismatched texture type";
    }
    if (tex->options_.compression == TextureType::DDS)
    {
        throw "Attemped to retrieve compressed texture type";
    }
    if (mip_level != 0 && tex->has_mipmaps_ == false)
    {
        throw "Attempted to retrieve mipmap of single level texture";
    }
    GLint width, height;
    GLint internal_format, input_format;
    GLenum input_type;
    glBindTexture(tex->type_, tex->texture_);
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mip_level, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mip_level, GL_TEXTURE_HEIGHT, &height);
    TranslateTextureFormat(tex->options_.format, &internal_format, &input_format, &input_type);

    PixelDataCubemap pixels;
    pixels.width = width;
    pixels.height = height;
    pixels.type = tex->options_;
    for (const auto& axis : { POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z })
    {
        pixels.pixels[axis].resize(width * height * (pixels.bits_per_pixel() / 8));
        glGetTexImage(TranslateAxisFormat(axis), mip_level, input_format, input_type, pixels.pixels[axis].data());
    }
    return pixels;
}

void RendererGL43::SetShaderData(ShaderDataResource* data_handle, std::size_t offset, std::size_t length, const void* data)
{
    auto data_buffer = resource_cast<ShaderDataResourceGL43*>(data_handle, id());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, data_buffer->buffer_);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, length, data);
}

void RendererGL43::GetShaderData(ShaderDataResource* data_handle, void* data)
{
    auto data_buffer = resource_cast<ShaderDataResourceGL43*>(data_handle, id());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, data_buffer->buffer_);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data_buffer->size_, data);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const float value)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const int value)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Matrix value)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Vector2 value)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Vector3 value)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Vector4 value)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value);
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const TextureResource* value, unsigned int texture_index)
{
    const TextureResourceGL43* tex = resource_cast<const TextureResourceGL43*>(value, id());
    glActiveTexture(GL_TEXTURE0 + texture_index);
    glBindTexture(tex->type_, tex->texture_);
    return SetShaderInput(program, name, static_cast<int>(texture_index));
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const ShaderDataResource* value)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->BindSSBO(name, resource_cast<const ShaderDataResourceGL43*>(value, id()));
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const float* value, std::size_t elements)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value, static_cast<GLsizei>(elements));
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const int* value, std::size_t elements)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value, static_cast<GLsizei>(elements));
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Matrix* value, std::size_t elements)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value, static_cast<GLsizei>(elements));
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Vector2* value, std::size_t elements)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value, static_cast<GLsizei>(elements));
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Vector3* value, std::size_t elements)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value, static_cast<GLsizei>(elements));
}

bool RendererGL43::SetShaderInput(ShaderResource* program, const char* name, const Vector4* value, std::size_t elements)
{
    auto prog = resource_cast<ShaderResourceGL43*>(program, id());
    return prog->SetUniform(name, value, static_cast<GLsizei>(elements));
}

bool RendererGL43::SetShaderOutput(ShaderResource* program, const char* name, const TextureResource* value, unsigned int texture_index, unsigned int mip_level)
{
    const TextureResourceGL43* tex = resource_cast<const TextureResourceGL43*>(value, id());
    GLboolean layered = (tex->type_ == GL_TEXTURE_3D) ? GL_TRUE : GL_FALSE;
    GLenum format;
    switch (tex->options_.format)
    {
    case TextureType::A8:
        format = GL_R8;
        break;
    case TextureType::R8G8_UINT:
        format = GL_RG8UI;
        break;
    case TextureType::R8G8B8A8_UINT:
        format = GL_RGBA8UI;
        break;
    case TextureType::R8G8B8A8:
        format = GL_RGBA8;
        break;
    case TextureType::R16G16:
        format = GL_RG16;
        break;
    case TextureType::R16G16B16A16:
        format = GL_RGBA16;
        break;
    case TextureType::A32:
        format = GL_R32F;
        break;
    case TextureType::R32G32:
        format = GL_RG32F;
        break;
    case TextureType::R32G32B32A32:
        format = GL_RGBA32F;
        break;
    case TextureType::NONE:
    case TextureType::R8G8B8:
    case TextureType::R8G8B8_UINT:
    case TextureType::R32G32B32:
    case TextureType::DEPTH:
    default:
        throw "Unsupported shader output format";
        break;
    }
    glBindImageTexture(texture_index, tex->texture_, mip_level, layered, 0, GL_WRITE_ONLY, format);
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

bool RendererGL43::SetViewport(units::pixel x, units::pixel y, units::pixel width, units::pixel height)
{
    glViewport(x, y, width, height);
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
    filetype = filetype.substr(filetype.size() - 4);

    if (filetype == ".dds")
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            throw "DDS texture file not found";
        }
        auto image_size = file.tellg();
        file.seekg(0, std::ios::beg);

        // 88 bytes is the size of a standard DDS texture header
        if (image_size < 88)
        {
            throw "Invalid .dds texture found";
        }
        data->pixels.resize(image_size);
        file.read(reinterpret_cast<char*>(data->pixels.data()), image_size);
        data->type.compression = TextureType::DDS;
        // The width and height come at the 16th and 12th byte of a DDS header respectively
        // https://msdn.microsoft.com/en-us/library/windows/desktop/bb943991(v=vs.85).aspx
        // Enjoy this ugly pointer casting dereferencing party for sad variables
        data->width = *reinterpret_cast<unsigned int*>(&data->pixels[16]);
        data->height = *reinterpret_cast<unsigned int*>(&data->pixels[12]);
    }
    else
    {
        data->type.compression = TextureType::AUTO;
        unsigned char* pixel_data = SOIL_load_image(filename.c_str(), &data->width, &data->height,
                                                    &channels, SOIL_LOAD_AUTO);
        if (pixel_data == nullptr)
        {
            return false;
        }
        auto pixel_data_length = data->width * data->height * channels;
        // Copy pixel buffer into our byte vector
        data->pixels.resize(pixel_data_length);
        memcpy(data->pixels.data(), pixel_data, pixel_data_length);
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

void RendererGL43::InitializeDebugOutput()
{
    // Debug output is disabled by default
    glEnable(GL_DEBUG_OUTPUT);
    // Debug output is delivered in the main thread
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // Filters out really common and generally useless info messages
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    // Debug output callback
    GLDEBUGPROC debug_out = [](GLenum source, GLenum type, GLuint id, GLenum severity,
                               GLsizei length, const GLchar* message, const void* user_param) {
        // Used for translating between enums and human readable strings
        std::unordered_map<GLenum, std::string> source_translations = {
            { GL_DEBUG_SOURCE_API, "API" },
            { GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM" },
            { GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER" },
            { GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY" },
            { GL_DEBUG_SOURCE_APPLICATION, "APPLICATION" },
            { GL_DEBUG_SOURCE_OTHER, "OTHER" }
        };
        std::unordered_map<GLenum, std::string> type_translations = {
            { GL_DEBUG_TYPE_ERROR, "ERROR" },
            { GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOUR" },
            { GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOUR" },
            { GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY" },
            { GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE" },
            { GL_DEBUG_TYPE_MARKER, "MARKER" },
            { GL_DEBUG_TYPE_PUSH_GROUP, "PUSH_GROUP" },
            { GL_DEBUG_TYPE_POP_GROUP, "POP_GROUP" },
            { GL_DEBUG_TYPE_OTHER, "OTHER" }
        };
        std::unordered_map<GLenum, std::string> severity_translations = {
            { GL_DEBUG_SEVERITY_HIGH, "HIGH" },
            { GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM" },
            { GL_DEBUG_SEVERITY_LOW, "LOW" },
            { GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION" }
        };
        log::Debug("[OpenGL:%s,%s,%s,ID_%i]%s\n", severity_translations[severity].c_str(),
                                                  source_translations[source].c_str(),
                                                  type_translations[type].c_str(),
                                                  id, message);
    };
    // Set the callback
    glDebugMessageCallback(debug_out, nullptr);
}
} // namespace blons
