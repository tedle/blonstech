#ifndef BLONSTECH_SHADER_H_
#define BLONSTECH_SHADER_H_


// Includes
#include "render.h"

class Shader
{
public:
    Shader();
    ~Shader();

    bool Load(HWND, RenderContext& context);
    void Finish(RenderContext& context);
    bool Render(int, TextureResource*, Matrix, Matrix, Matrix, RenderContext& context);

private:
    bool LoadShader(HWND, WCHAR*, WCHAR*, ShaderAttributeList, RenderContext& context);
    void FinishShader(RenderContext& context);

private:
    std::unique_ptr<ShaderResource> program_;
};

#endif