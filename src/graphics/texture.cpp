#include <graphics/texture.h>

namespace blons
{
Texture::Texture(std::string filename, Type type, RenderContext& context)
{
    std::unique_ptr<PixelData> tex_data(new PixelData);
    if (!context->LoadPixelData(filename, tex_data.get()))
    {
        throw "Failed to load texture";
    }
    Init(tex_data.get(), type, context);
}

Texture::Texture(PixelData* pixels, Type type, RenderContext& context)
{
    Init(pixels, type, context);
}

void Texture::Init(PixelData* pixels, Type type, RenderContext& context)
{
    texture_ = std::unique_ptr<TextureResource>(context->MakeTextureResource());
    if (texture_ == nullptr)
    {
        throw "Failed to load texture";
    }

    if (type == Type::SPRITE)
    {
        // No DDS compression or mipmaps + nearest neighbour filtering
        pixels->format = PixelData::RAW;
    }

    if (!context->RegisterTexture(texture_.get(), pixels))
    {
        throw "Failed to load texture";
    }

    info_.width = pixels->width;
    info_.height = pixels->height;
    info_.type = type;
}

Texture::~Texture()
{
}

Texture::Info Texture::info() const
{
    return info_;
}

const TextureResource* Texture::texture() const
{
    return texture_.get();
}
} // namespace blons
