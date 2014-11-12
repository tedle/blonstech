#ifndef BLONSTECH_GRAPHICS_RENDER_SHADER_H_
#define BLONSTECH_GRAPHICS_RENDER_SHADER_H_

// Includes
#include "render.h"

namespace blons
{
class Shader
{
public:
    Shader(const char* vertex_filename, const char* pixel_filename, ShaderAttributeList inputs, RenderContext& context);
    ~Shader();

    bool Render(int index_count, RenderContext& context);
    bool SetInput(const char* field, int value, RenderContext& context);
    bool SetInput(const char* field, Matrix value, RenderContext& context);
    bool SetInput(const char* field, Vector3 value, RenderContext& context);
    bool SetInput(const char* field, Vector4 value, RenderContext& context);
    bool SetInput(const char* field, TextureResource* value, RenderContext& context);
    ShaderResource* program();

private:
    std::unique_ptr<ShaderResource> program_;
};
} // namespace blons

#endif