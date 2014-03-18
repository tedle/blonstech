#include "texture.h"

Texture::Texture()
{
    texture_ = nullptr;
}

Texture::~Texture()
{
}

bool Texture::Init(const char* filename)
{
    texture_ = std::unique_ptr<TextureResource>(g_render->LoadDDSFile(filename));

    if (texture_ == nullptr)
    {
        return false;
    }

    return true;
}

void Texture::Finish()
{
    g_render->DestroyTextureResource(texture_.release());

    return;
}

TextureResource* Texture::GetTexture()
{
    return texture_.get();
}