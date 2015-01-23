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

    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glGetProcAddress("glBindFramebuffer");
    if (glBindFramebuffer == nullptr)
    {
        failed.push_back("glBindFramebuffer");
    }

    glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)glGetProcAddress("glBindRenderbuffer");
    if (glBindRenderbuffer == nullptr)
    {
        failed.push_back("glBindRenderbuffer");
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

    glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glGetProcAddress("glCheckFramebufferStatus");
    if (glCheckFramebufferStatus == nullptr)
    {
        failed.push_back("glCheckFramebufferStatus");
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

    glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glGetProcAddress("glDeleteFramebuffers");
    if (glDeleteFramebuffers == nullptr)
    {
        failed.push_back("glDeleteFramebuffers");
    }

    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glGetProcAddress("glDeleteProgram");
    if (glDeleteProgram == nullptr)
    {
        failed.push_back("glDeleteProgram");
    }

    glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)glGetProcAddress("glDeleteRenderbuffers");
    if (glDeleteRenderbuffers == nullptr)
    {
        failed.push_back("glDeleteRenderbuffers");
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

    glDrawBuffers = (PFNGLDRAWBUFFERSPROC)glGetProcAddress("glDrawBuffers");
    if (glDrawBuffers == nullptr)
    {
        failed.push_back("glDrawBuffers");
    }

    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glGetProcAddress("glEnableVertexAttribArray");
    if (glEnableVertexAttribArray == nullptr)
    {
        failed.push_back("glEnableVertexAttribArray");
    }

    glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glGetProcAddress("glFramebufferRenderbuffer");
    if (glFramebufferRenderbuffer == nullptr)
    {
        failed.push_back("glFramebufferRenderbuffer");
    }

    glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)glGetProcAddress("glFramebufferTexture");
    if (glFramebufferTexture == nullptr)
    {
        failed.push_back("glFramebufferTexture");
    }

    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glGetProcAddress("glFramebufferTexture2D");
    if (glFramebufferTexture2D == nullptr)
    {
        failed.push_back("glFramebufferTexture2D");
    }

    glGenBuffers = (PFNGLGENBUFFERSPROC)glGetProcAddress("glGenBuffers");
    if (glGenBuffers == nullptr)
    {
        failed.push_back("glGenBuffers");
    }

    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glGetProcAddress("glGenFramebuffers");
    if (glGenFramebuffers == nullptr)
    {
        failed.push_back("glGenFramebuffers");
    }

    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glGetProcAddress("glGenerateMipmap");
    if (glGenerateMipmap == nullptr)
    {
        failed.push_back("glGenerateMipmap");
    }

    glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)glGetProcAddress("glGenRenderbuffers");
    if (glGenRenderbuffers == nullptr)
    {
        failed.push_back("glGenRenderbuffers");
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

    glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)glGetProcAddress("glRenderbufferStorage");
    if (glRenderbufferStorage == nullptr)
    {
        failed.push_back("glRenderbufferStorage");
    }

    glShaderSource = (PFNGLSHADERSOURCEPROC)glGetProcAddress("glShaderSource");
    if (glShaderSource == nullptr)
    {
        failed.push_back("glShaderSource");
    }

    glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)glGetProcAddress("glTexImage2DMultisample");
    if (glTexImage2DMultisample == nullptr)
    {
        failed.push_back("glTexImage2DMultisample");
    }

    glTexImage3D = (PFNGLTEXIMAGE3DPROC)glGetProcAddress("glTexImage3D");
    if (glTexImage3D == nullptr)
    {
        failed.push_back("glTexImage3D");
    }

    glUniform1f = (PFNGLUNIFORM1FPROC)glGetProcAddress("glUniform1f");
    if (glUniform1f == nullptr)
    {
        failed.push_back("glUniform1f");
    }

    glUniform1i = (PFNGLUNIFORM1IPROC)glGetProcAddress("glUniform1i");
    if (glUniform1i == nullptr)
    {
        failed.push_back("glUniform1i");
    }

    glUniform2fv = (PFNGLUNIFORM2FVPROC)glGetProcAddress("glUniform2fv");
    if (glUniform2fv == nullptr)
    {
        failed.push_back("glUniform2fv");
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
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLDRAWBUFFERSPROC glDrawBuffers;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
PFNGLTEXIMAGE3DPROC glTexImage3D;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2FVPROC glUniform2fv;
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