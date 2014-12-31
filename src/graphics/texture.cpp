////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/graphics/texture.h>

// Local Includes
#include "resource.h"

namespace blons
{
Texture::Texture(std::string filename, Type type, RenderContext& context)
{
    if (!Init(filename, type, context))
    {
        throw "Failed to load texture";
    }
}

bool Texture::Init(std::string filename, Type type, RenderContext& context)
{
    filename_ = filename;
    pixel_data_ = nullptr;

    texture_ = resource::LoadTexture(filename, type, &info_, context);
    if (texture_ == nullptr)
    {
        return false;
    }
    return true;
}

Texture::Texture(PixelData* pixels, Type type, RenderContext& context)
{
    if (!Init(pixels, type, context))
    {
        throw "Failed to load texture";
    }
}

bool Texture::Init(PixelData* pixels, Type type, RenderContext& context)
{
    filename_ = "";
    pixel_data_ = pixels;

    texture_.reset(context->MakeTextureResource());
    if (texture_ == nullptr)
    {
        return false;
    }

    if (type == Type::SPRITE)
    {
        // No DDS compression or mipmaps + nearest neighbour filtering
        pixels->format = PixelData::RAW;
    }

    if (!context->RegisterTexture(texture_.get(), pixels))
    {
        return false;
    }

    info_.width = pixels->width;
    info_.height = pixels->height;
    info_.type = type;

    return true;
}

bool Texture::Reload(RenderContext& context)
{
    if (filename_.length() > 0)
    {
        return Init(filename_, info_.type, context);
    }

    else if (pixel_data_ != nullptr)
    {
        return Init(pixel_data_, info_.type, context);
    }

    return false;
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
