#include "texture.h"

namespace blons
{
Texture::Texture()
{
    texture_ = nullptr;
}

Texture::~Texture()
{
}

bool Texture::Load(const char* filename, Type type, RenderContext& context)
{
    texture_ = std::unique_ptr<TextureResource>(context->LoadDDSFile(filename));

    if (texture_ == nullptr)
    {
        return false;
    }

    type_ = type;

    return true;
}

void Texture::Finish(RenderContext& context)
{
    context->DestroyTextureResource(texture_.release());

    return;
}

TextureResource* Texture::texture()
{
    return texture_.get();
}
} // namespace blons