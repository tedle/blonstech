#include "texture.h"

Texture::Texture()
{
    texture_ = nullptr;
}

Texture::~Texture()
{
}

bool Texture::Init(WCHAR* filename)
{
    texture_ = g_render->LoadDDSFile(filename);

    if(texture_ == nullptr)
        return false;

    return true;
}

void Texture::Finish()
{
    g_render->DestroyTextureResource(texture_);

    return;
}

TextureResource* Texture::GetTexture()
{
    return texture_;
}