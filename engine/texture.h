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
    Texture();
    ~Texture();

    enum Format {NONE, DXT5, TGA};
    enum Type {DIFFUSE, NORMAL, LIGHT};

    bool Load(const char* filename, Type type, RenderContext& context);
    void Finish(RenderContext& context);

    TextureResource* texture();

private:
    std::unique_ptr<TextureResource> texture_;
    Format format_;
    Type type_;
};
} // namespace blons

#endif