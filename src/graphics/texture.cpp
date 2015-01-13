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

    auto tex = resource::LoadTexture(filename, type, context);
    texture_ = std::move(tex.texture);
    info_ = tex.info;
    if (texture_ == nullptr)
    {
        return false;
    }
    return true;
}

Texture::Texture(const PixelData& pixels, Type type, RenderContext& context)
{
    if (!Init(pixels, type, context))
    {
        throw "Failed to load texture";
    }
}

bool Texture::Init(const PixelData& pixels, Type type, RenderContext& context)
{
    filename_ = "";
    pixel_data_.reset(new PixelData(pixels));

    // Make the actual texture
    texture_.reset(context->MakeTextureResource());
    if (texture_ == nullptr)
    {
        return false;
    }

    if (type == Type::SPRITE)
    {
        // No DDS compression or mipmaps + nearest neighbour filtering
        pixel_data_->compression = PixelData::RAW;
        pixel_data_->hint.filter = TextureHint::NEAREST;
    }
    else
    {
        pixel_data_->hint.filter = TextureHint::LINEAR;
    }

    // Get the most out of our lightmaps...
    if (type == Type::LIGHT)
    {
        pixel_data_->compression = PixelData::RAW;
    }

    if (!context->RegisterTexture(texture_.get(), pixel_data_.get()))
    {
        return false;
    }

    info_.width = pixel_data_->width;
    info_.height = pixel_data_->height;
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
        return Init(*pixel_data_, info_.type, context);
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
