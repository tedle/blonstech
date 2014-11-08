#ifndef BLONSTECH_TEXTURE_H_
#define BLONSTECH_TEXTURE_H_

// Includes
#include <memory>
// Local Includes
#include "render.h"

namespace blons
{
class Texture
{
public:
    enum Format {NONE, DXT5, TGA};
    enum Type {DIFFUSE, NORMAL, LIGHT, SPRITE};

    Texture(const char* filename, Type type, RenderContext& context);
    Texture(PixelData* pixels, Type type, RenderContext& context);
    ~Texture();

    TextureResource* texture();

private:
    void Init(PixelData* pixels, Type type, RenderContext& context);
    std::unique_ptr<TextureResource> texture_;
    Format format_;
    Type type_;
};
} // namespace blons

#endif