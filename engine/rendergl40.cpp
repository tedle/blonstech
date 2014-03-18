#include "rendergl40.h"

RenderGL40::RenderGL40()
{
}

RenderGL40::~RenderGL40()
{
}

bool RenderGL40::Init(int screen_width, int screen_height, bool vsync, HWND hwnd,
                      bool fullscreen, float screen_depth, float screen_near)
{

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
        return false;
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
        return false;
    }

    // Creating a false context, to get gl functions, to create a better context...
    auto dummy_render_context = wglCreateContext(dummy_device_context);
    wglMakeCurrent(dummy_device_context, dummy_render_context);

    // TODO: actually check we get the needed functions
    if (LoadWGLFunctions().size() > 0)
    {
        return false;
    }

    // Clean up dummy context
    wglDeleteContext(dummy_render_context);
    ReleaseDC(dummy_hwnd, dummy_device_context);
    DestroyWindow(dummy_hwnd);

    // Get the real device context handle
    device_context_ = GetDC(hwnd);

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
        return false;
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
        return false;
    }

    // Clean up the dummy context
    if (!wglMakeCurrent(device_context_, render_context_))
    {
        return false;
    }

    // Finally load the rest of our functions
    if (LoadGLFunctions().size() > 0)
    {
        return false;
    }

    // Projection matrix (3D space->2D screen)
    float fov = kPi / 4.0f;
    float screen_aspect = (float)screen_width / (float)screen_height;

    proj_matrix_ = MatrixPerspectiveFov(fov, screen_aspect, screen_near, screen_depth);

    // Ortho projection matrix (for 2d stuff, shadow maps, etc)
    ortho_matrix_ = MatrixOrthographic((float)screen_width, (float)screen_height, screen_near, screen_depth);

    // Grab video card info
    video_card_desc_ = (char*)glGetString(GL_VENDOR);
    video_card_desc_ += " ";
    video_card_desc_ += (char*)glGetString(GL_RENDERER);
    // This isn't supported in OpenGL
    video_card_memory_ = 0;

    // Enable depth testing, with a default of 1.0
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    // Configure how we render tris
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    // Configure vsync (please be false)
    vsync_ = vsync;
    if (!wglSwapIntervalEXT(vsync))
    {
        return false;
    }

    return true;
}

void RenderGL40::Finish()
{
    // Reset the current context before deleting it
    wglMakeCurrent(device_context_, nullptr);
    wglDeleteContext(render_context_);
}

void RenderGL40::BeginScene()
{
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderGL40::EndScene()
{
    SwapBuffers(device_context_);
}

void* RenderGL40::CreateBufferResource()
{
    return malloc(sizeof(BufferResourceGL40));
}

void* RenderGL40::CreateTextureResource()
{
    return malloc(sizeof(TextureResourceGL40));
}

void* RenderGL40::CreateShaderResource()
{
    return malloc(sizeof(ShaderResourceGL40));
}

void RenderGL40::DestroyBufferResource(BufferResource* buffer)
{
    BufferResourceGL40* buf = static_cast<BufferResourceGL40*>(buffer);
    if (buf->type_ == BufferResourceGL40::VERTEX_BUFFER)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else if (buf->type_ == BufferResourceGL40::INDEX_BUFFER)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glDeleteBuffers(1, &buf->buffer_);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &buf->vertex_array_id_);
}

void RenderGL40::DestroyTextureResource(TextureResource* texture)
{

}

void RenderGL40::DestroyShaderResource(ShaderResource* shader)
{
    ShaderResourceGL40* program = static_cast<ShaderResourceGL40*>(shader);
    glDetachShader(program->program_, program->vertex_shader_);
    glDetachShader(program->program_, program->frag_shader_);

    glDeleteShader(program->vertex_shader_);
    glDeleteShader(program->frag_shader_);

    glDeleteProgram(program->program_);
}

bool RenderGL40::RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
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

    // Enable pos and uv inputs ??
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Layout the Vertex struct type to gpu vertex attributes
    // Position declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // UV declaration
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf->buffer_);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));

    // Setup the index buffer
    glGenBuffers(1, &index_buf->buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf->buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    return true;
}

void RenderGL40::RegisterTexture()
{
    return;
}

bool RenderGL40::RegisterShader(ShaderResource* program,
                                WCHAR* vertex_filename, WCHAR* pixel_filename)
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
    glBindAttribLocation(shader->program_, 0, "input_pos");
    glBindAttribLocation(shader->program_, 1, "input_uv");
    glLinkProgram(shader->program_);

    // Check that everything went OK
    int link_result;
    glGetProgramiv(shader->program_, GL_LINK_STATUS, &link_result);
    if (!link_result)
    {
        LogCompileErrors(shader->program_, true);
    }

    return true;
}

void RenderGL40::RenderShader(ShaderResource* program, int index_count)
{
    ShaderResourceGL40* shader = static_cast<ShaderResourceGL40*>(program);

    // Bind our shader then do the draw call
    glUseProgram(shader->program_);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}

void RenderGL40::SetModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)
{
    BufferResourceGL40* vertex_buf = static_cast<BufferResourceGL40*>(vertex_buffer);
    glBindVertexArray(vertex_buf->vertex_array_id_);
}

bool RenderGL40::SetShaderInputs(ShaderResource* program, TextureResource* texture,
                                 Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix)
{
    TextureResourceGL40* tex = static_cast<TextureResourceGL40*>(texture);
    ShaderResourceGL40* prog = static_cast<ShaderResourceGL40*>(program);
    GLuint loc;

    // Bind our uniform variables to the shader
    loc = glGetUniformLocation(prog->program_, "world_matrix");
    if (loc < 0)
    {
        return false;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)world_matrix.m);

    loc = glGetUniformLocation(prog->program_, "view_matrix");
    if (loc < 0)
    {
        return false;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)view_matrix.m);

    loc = glGetUniformLocation(prog->program_, "proj_matrix");
    if (loc < 0)
    {
        return false;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)proj_matrix.m);

    loc = glGetUniformLocation(prog->program_, "shader_texture");
    if (loc < 0)
    {
        return false;
    }
    glUniform1i(loc, tex->texture_unit_);
    glBindTexture(GL_TEXTURE_2D, tex->texture_);

    return true;
}

Matrix RenderGL40::GetProjectionMatrix()
{
    return proj_matrix_;
}

Matrix RenderGL40::GetOrthoMatrix()
{
    return ortho_matrix_;
}

void RenderGL40::GetVideoCardInfo(char* name, int& memory)
{
    strcpy_s(name, 128, video_card_desc_.c_str());
    memory = video_card_memory_;
    return;
}

TextureResource* RenderGL40::LoadDDSFile(const char* filename)
{
    TextureResourceGL40* texture = new TextureResourceGL40;
    texture->texture_unit_ = 0;

	// Set the texture unit in which to store the data.
	glActiveTexture(GL_TEXTURE0 + texture->texture_unit_);

	// Generate an ID for the texture.
	glGenTextures(1, &texture->texture_);

	// Bind the texture as a 2D texture.
	glBindTexture(GL_TEXTURE_2D, texture->texture_);

	// Set the texture to repeat when sampled outside UV range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set the texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Load the texture onto GPU
    texture->texture_ = SOIL_load_OGL_texture(filename, 0, texture->texture_, SOIL_FLAG_DDS_LOAD_DIRECT);

    // Re enable this for non-dds textures
	// glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

void RenderGL40::LogCompileErrors(GLuint resource, bool is_shader)
{
    // Grab the compile errors
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
    // Write 'em to disk
    std::ofstream fout("shader.log");
    fout.write(compile_errors.get(), buffer_size);
    fout.close();

    return;
}