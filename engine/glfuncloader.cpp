#include "glfuncloader.h"

void* glGetProcAddress(const char* func)
{
    // Windows implementation
    return wglGetProcAddress(func);
}

std::vector<std::string> LoadGLFunctions()
{
    std::vector<std::string> failed;
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

    return failed;
}

// Have to define these all, even though we can't use em yet...
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;