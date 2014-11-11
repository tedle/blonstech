#ifndef BLONSTECH_TEXTURE_H_
#define BLONSTECH_TEXTURE_H_

// Includes
#include <memory>
// Local Includes
#include "render/render.h"

namespace blons
{
class Texture
{
public:
    enum Type {DIFFUSE, NORMAL, LIGHT, SPRITE};
    struct Info
    {
        std::size_t width, height;
        Type type;
    };

    Texture(const char* filename, Type type, RenderContext& context);
    Texture(PixelData* pixels, Type type, RenderContext& context);
    ~Texture();

    Info info();
    TextureResource* texture();

private:
    void Init(PixelData* pixels, Type type, RenderContext& context);
    std::unique_ptr<TextureResource> texture_;
    Info info_;
};
} // namespace blons

#endif