#include "texture.h"

Texture::Texture()
{
    texture_ = nullptr;
}

Texture::~Texture()
{
}

bool Texture::Init(ID3D11Device* device, WCHAR* filename)
{
    HRESULT result;

    result = DirectX::CreateDDSTextureFromFile(device, filename, nullptr, &texture_, 0);
    if(FAILED(result))
        return false;

    return true;
}

void Texture::Finish()
{
    if(texture_)
    {
        texture_->Release();
        texture_ = nullptr;
    }

    return;
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
    return texture_;
}