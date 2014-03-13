#include "rendergl40.h"

RenderGL40::RenderGL40()
{
}

RenderGL40::~RenderGL40()
{
}

bool RenderGL40::Init(int screen_width, int screen_height, bool vsync, HWND hwnd,
                      bool fullscreen, float depth, float near)
{

    // Defining the pixel format we want OpenGL to use
    const int color_depth = 32;
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

    // Get the real device context handle
    device_context_ = GetDC(hwnd);

    // Set the proper pixel format
    unsigned int num_formats;
    const int pixel_attributes[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, color_depth,
        WGL_DEPTH_BITS_ARB, depth_bits,
        WGL_STENCIL_BITS_ARB, stencil_bits,
        0
    };
    pixel_format = wglChoosePixelFormatARB(device_context_, pixel_attributes, nullptr, 1, &pixel_format, &num_formats);
    if (!pixel_format || !SetPixelFormat(device_context_, pixel_format, &pfd))
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

    wglMakeCurrent(device_context_, render_context_);
    wglDeleteContext(dummy_render_context);
    DestroyWindow(dummy_hwnd);

    // Finally load the rest of our functions
    if (LoadGLFunctions().size() > 0)
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

}

void RenderGL40::EndScene()
{

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

}

void RenderGL40::DestroyTextureResource(TextureResource* texture)
{

}

void RenderGL40::DestroyShaderResource(ShaderResource* shader)
{

}

bool RenderGL40::RegisterMesh(BufferResource* vertex_buffer, BufferResource* index_buffer,
                              Vertex* vertices, unsigned int vert_count,
                              unsigned long* indices, unsigned int index_count)
{
    return false;
}

void RenderGL40::RegisterTexture()
{

}

bool RenderGL40::RegisterShader(ShaderResource* program,
                                WCHAR* vertex_filename, WCHAR* pixel_filename)
{
    return false;
}

void RenderGL40::RenderShader(ShaderResource* program, int index_count)
{

}

void RenderGL40::SetModelBuffer(BufferResource* vertex_buffer, BufferResource* index_buffer)
{

}

bool RenderGL40::SetShaderInputs(ShaderResource* program, TextureResource* texture,
                                 Matrix world_matrix, Matrix view_matrix, Matrix proj_matrix)
{
    return false;
}

Matrix RenderGL40::GetProjectionMatrix()
{
    return proj_matrix_;
}

Matrix RenderGL40::GetOrthoMatrix()
{
    return ortho_matrix_;
}

void RenderGL40::GetVideoCardInfo(char* buffer, int& len_buffer)
{

}

TextureResource* RenderGL40::LoadDDSFile(WCHAR* filename)
{
    return nullptr;
}