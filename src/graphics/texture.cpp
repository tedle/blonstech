////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
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
Texture::Texture(std::string filename, TextureType::Options options)
{
    Init(filename, options);
}

void Texture::Init(std::string filename, TextureType::Options options)
{
    filename_ = filename;
    pixel_data_ = nullptr;

    auto tex = resource::LoadTexture(filename, options);
    texture_ = std::move(tex.texture);
    info_ = tex.info;
    if (texture_ == nullptr)
    {
        throw "Failed to load texture";
    }
}

Texture::Texture(const PixelData& pixels)
{
    Init(pixels);
}

void Texture::Init(const PixelData& pixels)
{
    auto context = render::context();
    filename_ = "";
    pixel_data_.reset(new PixelData(pixels));

    // Make the actual texture
    texture_.reset(context->MakeTextureResource());
    if (texture_ == nullptr)
    {
        throw "Failed to allocate texture resource";
    }

    if (!context->RegisterTexture(texture_.get(), pixel_data_.get()))
    {
        throw "Failed to register texture resource";
    }

    info_.width = pixel_data_->width;
    info_.height = pixel_data_->height;
    info_.type = pixel_data_->type;
}

void Texture::Reload()
{
    if (filename_.length() > 0)
    {
        Init(filename_, { info_.type.compression, info_.type.filter, info_.type.wrap });
    }

    else if (pixel_data_ != nullptr)
    {
        Init(*pixel_data_);
    }
}

const Texture::Info* Texture::info() const
{
    return &info_;
}

const PixelData* Texture::pixels(bool force_gpu_sync)
{
    if (force_gpu_sync || pixel_data_ == nullptr)
    {
        pixel_data_.reset(new PixelData(render::context()->GetTextureData(texture_.get(), 0)));
    }
    return pixel_data_.get();
}

const TextureResource* Texture::texture() const
{
    return texture_.get();
}
} // namespace blons
