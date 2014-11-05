#include "texture.h"

namespace blons
{
Texture::Texture(const char* filename, Type type, RenderContext& context)
{
    texture_ = std::unique_ptr<TextureResource>(context->LoadDDSFile(filename));

    if (texture_ == nullptr)
    {
        throw "Failed to load texture";
    }

    type_ = type;
}

Texture::~Texture()
{
}

TextureResource* Texture::texture()
{
    return texture_.get();
}
} // namespace blons
