#ifndef BLONSTECH_TEXTURE_H_
#define BLONSTECH_TEXTURE_H_

#include <d3d11.h>
#include "external\DDSTextureLoader.h"

class Texture
{
public:
    Texture();
    ~Texture();

    bool Init(ID3D11Device*, WCHAR*);
    void Finish();

    ID3D11ShaderResourceView* GetTexture();

private:
    ID3D11ShaderResourceView* texture_;
};

#endif