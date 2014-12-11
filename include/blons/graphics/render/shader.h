#ifndef BLONSTECH_GRAPHICS_RENDER_SHADER_H_
#define BLONSTECH_GRAPHICS_RENDER_SHADER_H_

// Includes
#include <blons/graphics/render/render.h>

namespace blons
{
class Shader
{
public:
    Shader(std::string vertex_filename, std::string pixel_filename, ShaderAttributeList inputs, RenderContext& context);
    ~Shader();

    bool Render(unsigned int index_count, RenderContext& context);
    bool SetInput(const char* field, int value, RenderContext& context);
    bool SetInput(const char* field, Matrix value, RenderContext& context);
    bool SetInput(const char* field, Vector3 value, RenderContext& context);
    bool SetInput(const char* field, Vector4 value, RenderContext& context);
    bool SetInput(const char* field, const TextureResource* value, RenderContext& context);

    ShaderResource* program() const;

private:
    std::unique_ptr<ShaderResource> program_;
};
} // namespace blons
#endif