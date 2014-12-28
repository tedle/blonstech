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

#include "glfuncloader.h"

namespace blons
{
void* glGetProcAddress(const char* func)
{
    // Windows implementation
    return wglGetProcAddress(func);
}

std::vector<std::string> LoadGLFunctions()
{
    std::vector<std::string> failed;

    glActiveTexture = (PFNGLACTIVETEXTUREPROC)glGetProcAddress("glActiveTexture");
    if (glActiveTexture == nullptr)
    {
        failed.push_back("glActiveTexture");
    }

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

    glBindBuffer = (PFNGLBINDBUFFERPROC)glGetProcAddress("glBindBuffer");
    if (glBindBuffer == nullptr)
    {
        failed.push_back("glBindBuffer");
    }

    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glGetProcAddress("glBindVertexArray");
    if (glBindVertexArray == nullptr)
    {
        failed.push_back("glBindVertexArray");
    }

    glBufferData = (PFNGLBUFFERDATAPROC)glGetProcAddress("glBufferData");
    if (glBufferData == nullptr)
    {
        failed.push_back("glBufferData");
    }

    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glGetProcAddress("glBufferSubData");
    if (glBufferSubData == nullptr)
    {
        failed.push_back("glBufferSubData");
    }

    glMapBuffer = (PFNGLMAPBUFFERPROC)glGetProcAddress("glMapBuffer");
    if (glMapBuffer == nullptr)
    {
        failed.push_back("glMapBuffer");
    }

    glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)glGetProcAddress("glUnmapBuffer");
    if (glUnmapBuffer == nullptr)
    {
        failed.push_back("glUnmapBuffer");
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

    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glGetProcAddress("glDeleteBuffers");
    if (glDeleteBuffers == nullptr)
    {
        failed.push_back("glDeleteBuffers");
    }

    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glGetProcAddress("glDeleteProgram");
    if (glDeleteProgram == nullptr)
    {
        failed.push_back("glDeleteProgram");
    }

    glDeleteShader = (PFNGLDELETESHADERPROC)glGetProcAddress("glDeleteShader");
    if (glDeleteShader == nullptr)
    {
        failed.push_back("glDeleteShader");
    }

    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glGetProcAddress("glDeleteVertexArrays");
    if (glDeleteVertexArrays == nullptr)
    {
        failed.push_back("glDeleteVertexArrays");
    }

    glDetachShader = (PFNGLDETACHSHADERPROC)glGetProcAddress("glDetachShader");
    if (glDetachShader == nullptr)
    {
        failed.push_back("glDetachShader");
    }

    glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glGetProcAddress("glDisableVertexAttribArray");
    if (glDisableVertexAttribArray == nullptr)
    {
        failed.push_back("glDisableVertexAttribArray");
    }

    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glGetProcAddress("glEnableVertexAttribArray");
    if (glEnableVertexAttribArray == nullptr)
    {
        failed.push_back("glEnableVertexAttribArray");
    }

    glGenBuffers = (PFNGLGENBUFFERSPROC)glGetProcAddress("glGenBuffers");
    if (glGenBuffers == nullptr)
    {
        failed.push_back("glGenBuffers");
    }

    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glGetProcAddress("glGenerateMipmap");
    if (glGenerateMipmap == nullptr)
    {
        failed.push_back("glGenerateMipmap");
    }

    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glGetProcAddress("glGenVertexArrays");
    if (glGenVertexArrays == nullptr)
    {
        failed.push_back("glGenVertexArrays");
    }

    glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)glGetProcAddress("glGetBufferParameteriv");
    if (glGetBufferParameteriv == nullptr)
    {
        failed.push_back("glGetBufferParameteriv");
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

    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glGetProcAddress("glGetUniformLocation");
    if (glGetUniformLocation == nullptr)
    {
        failed.push_back("glGetUniformLocation");
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

    glUniform1i = (PFNGLUNIFORM1IPROC)glGetProcAddress("glUniform1i");
    if (glUniform1i == nullptr)
    {
        failed.push_back("glUniform1i");
    }

    glUniform3fv = (PFNGLUNIFORM3FVPROC)glGetProcAddress("glUniform3fv");
    if (glUniform3fv == nullptr)
    {
        failed.push_back("glUniform3fv");
    }

    glUniform4fv = (PFNGLUNIFORM4FVPROC)glGetProcAddress("glUniform4fv");
    if (glUniform4fv == nullptr)
    {
        failed.push_back("glUniform4fv");
    }

    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glGetProcAddress("glUniformMatrix4fv");
    if (glUniformMatrix4fv == nullptr)
    {
        failed.push_back("glUniformMatrix4fv");
    }

    glUseProgram = (PFNGLUSEPROGRAMPROC)glGetProcAddress("glUseProgram");
    if (glUseProgram == nullptr)
    {
        failed.push_back("glUseProgram");
    }

    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glGetProcAddress("glVertexAttribPointer");
    if (glVertexAttribPointer == nullptr)
    {
        failed.push_back("glVertexAttribPointer");
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
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
// WGL
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
} // namespace blons