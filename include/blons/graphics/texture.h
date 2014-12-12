#ifndef BLONSTECH_GRAPHICS_TEXTURE_H_
#define BLONSTECH_GRAPHICS_TEXTURE_H_

// Includes
#include <memory>
// Public Includes
#include <blons/graphics/render/render.h>

namespace blons
{
class Texture
{
public:
    enum Type {DIFFUSE, NORMAL, LIGHT, SPRITE};
    struct Info
    {
        units::pixel width, height;
        Type type;
    };

    Texture(std::string filename, Type type, RenderContext& context);
    Texture(PixelData* pixels, Type type, RenderContext& context);
    ~Texture();

    Info info() const;
    const TextureResource* texture() const;

private:
    void Init(PixelData* pixels, Type type, RenderContext& context);
    std::unique_ptr<TextureResource> texture_;
    Info info_;
};
} // namespace blons

#endif // BLONSTECH_GRAPHICS_TEXTURE_H_