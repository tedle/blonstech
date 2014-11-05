#ifndef BLONSTECH_SHADER_H_
#define BLONSTECH_SHADER_H_


// Includes
#include "render.h"

namespace blons
{
class Shader
{
public:
    Shader(HWND, RenderContext& context);
    ~Shader();

    bool Render(int, TextureResource*, Matrix, Matrix, Matrix, RenderContext& context);

private:
    std::unique_ptr<ShaderResource> program_;
};
} // namespace blons

#endif