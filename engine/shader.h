#ifndef BLONSTECH_SHADER_H_
#define BLONSTECH_SHADER_H_


// Includes
#include "render.h"

class Shader
{
public:
    Shader();
    ~Shader();

    bool Init(HWND);
    void Finish();
    bool Render(int, TextureResource*, Matrix, Matrix, Matrix);

private:
    bool InitShader(HWND, WCHAR*, WCHAR*);
    void FinishShader();

    bool SetShaderParams(Matrix, Matrix, Matrix, TextureResource*);

private:
    ShaderResource* program_;
};

#endif