#ifndef BLONSTECH_TEXTURE_H_
#define BLONSTECH_TEXTURE_H_

#include "render.h"

class Texture
{
public:
    Texture();
    ~Texture();

    bool Init(WCHAR*);
    void Finish();

    TextureResource* GetTexture();

private:
    TextureResource* texture_;
};

#endif