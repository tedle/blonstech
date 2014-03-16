#include "glfuncloader.h"

void* glGetProcAddress(const char* func)
{
    // Windows implementation
    return wglGetProcAddress(func);
}

std::vector<std::string> LoadGLFunctions()
{
    std::vector<std::string> failed;

    glAttachShader = (PFNGLATTACHSHADERPROC)glGetProcAddress("glAttachShader");
    if (glAttachShader == nullptr)
    {
        failed.push_back("glAttachShader");
    }

    glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)glGetProcAddress("glBindAttribLocation");
    if (glBindAttribLocation == nullptr)
    {
        failed.push_back("glBindAttribLocation");
    }

    glCompileShader = (PFNGLCOMPILESHADERPROC)glGetProcAddress("glCompileShader");
    if (glCompileShader == nullptr)
    {
        failed.push_back("glCompileShader");
    }

    glCreateProgram = (PFNGLCREATEPROGRAMPROC)glGetProcAddress("glCreateProgram");
    if (glCreateProgram == nullptr)
    {
        failed.push_back("glCreateProgram");
    }

    glCreateShader = (PFNGLCREATESHADERPROC)glGetProcAddress("glCreateShader");
    if (glCreateShader == nullptr)
    {
        failed.push_back("glCreateShader");
    }

    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glGetProcAddress("glGetProgramiv");
    if (glGetProgramiv == nullptr)
    {
        failed.push_back("glGetProgramiv");
    }

    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glGetProcAddress("glGetProgramInfoLog");
    if (glGetProgramInfoLog == nullptr)
    {
        failed.push_back("glGetProgramInfoLog");
    }

    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glGetProcAddress("glGetShaderInfoLog");
    if (glGetShaderInfoLog == nullptr)
    {
        failed.push_back("glGetShaderInfoLog");
    }

    glGetShaderiv = (PFNGLGETSHADERIVPROC)glGetProcAddress("glGetShaderiv");
    if (glGetShaderiv == nullptr)
    {
        failed.push_back("glGetShaderiv");
    }

    glLinkProgram = (PFNGLLINKPROGRAMPROC)glGetProcAddress("glLinkProgram");
    if (glLinkProgram == nullptr)
    {
        failed.push_back("glLinkProgram");
    }

    glShaderSource = (PFNGLSHADERSOURCEPROC)glGetProcAddress("glShaderSource");
    if (glShaderSource == nullptr)
    {
        failed.push_back("glShaderSource");
    }
    return failed;
}

std::vector<std::string> LoadWGLFunctions()
{
    std::vector<std::string> failed;

    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)glGetProcAddress("wglChoosePixelFormatARB");
    if (wglChoosePixelFormatARB == nullptr)
    {
        failed.push_back("wglChoosePixelFormatARB");
    }

    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)glGetProcAddress("wglCreateContextAttribsARB");
    if (wglCreateContextAttribsARB == nullptr)
    {
        failed.push_back("wglCreateContextAttribsARB");
    }

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)glGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT == nullptr)
    {
        failed.push_back("wglSwapIntervalEXT");
    }
    return failed;
}

// Have to define these all, even though we can't use em yet...
// GL
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
// WGL
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;