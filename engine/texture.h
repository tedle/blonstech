#ifndef BLONSTECH_TEXTURE_H_
#define BLONSTECH_TEXTURE_H_

// Includes
#include <memory>
// Local Includes
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
    std::unique_ptr<TextureResource> texture_;
};

#endif