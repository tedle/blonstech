#ifndef BLONSTECH_GLFUNCLOADER_H_
#define BLONSTECH_GLFUNCLOADER_H_

// Includes
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glext.h>
#include <gl/wglext.h>
#include <vector>
#include <string>

// Platform independant function loader
void* glGetProcAddress(const char* func);

// Returns list of function names that failed to load
// Never call these without a GL context setup
std::vector<std::string> LoadGLFunctions();
std::vector<std::string> LoadWGLFunctions();

// List of function prototypes... brace yourself
extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

#endif