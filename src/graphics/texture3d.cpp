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

#include <blons/graphics/texture3d.h>

// Local Includes
#include "resource.h"

namespace blons
{
Texture3D::Texture3D(const PixelData3D& pixels)
{
    Init(pixels);
}

void Texture3D::Init(const PixelData3D& pixels)
{
    auto context = render::context();
    pixel_data_.reset(new PixelData3D(pixels));

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
    info_.depth = pixel_data_->depth;
    info_.type = pixel_data_->type;
}

void Texture3D::Reload()
{
    Init(*pixel_data_);
}

Texture3D::Info Texture3D::info() const
{
    return info_;
}

const TextureResource* Texture3D::texture() const
{
    return texture_.get();
}
} // namespace blons
