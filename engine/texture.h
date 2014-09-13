#ifndef BLONSTECH_TEXTURE_H_
#define BLONSTECH_TEXTURE_H_

// Includes
#include <memory>
// Local Includes
#include "render.h"

class Texture
{
public:
    enum Format {NONE, DXT5, TGA};
    enum Type {DIFFUSE, NORMAL, LIGHT};

    Texture(const char* filename, Type type, RenderContext& context);
    ~Texture();

    TextureResource* texture();

private:
    std::unique_ptr<TextureResource> texture_;
    Format format_;
    Type type_;
};

#endif