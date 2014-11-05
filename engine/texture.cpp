#include "texture.h"

namespace blons
{
Texture::Texture(const char* filename, Type type, RenderContext& context)
{
    texture_ = std::unique_ptr<TextureResource>(context->CreateTextureResource());
    if (texture_ == nullptr)
    {
        throw "Failed to load texture";
    }

    std::unique_ptr<PixelData> tex_data(new PixelData);
    if (!context->LoadPixelData(filename, tex_data.get()))
    {
        throw "Failed to load texture";
    }
    if (!context->RegisterTexture(texture_.get(), tex_data.get()))
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
