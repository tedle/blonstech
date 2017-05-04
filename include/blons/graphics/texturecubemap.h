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

#ifndef BLONSTECH_GRAPHICS_TEXTURECUBEMAP_H_
#define BLONSTECH_GRAPHICS_TEXTURECUBEMAP_H_

// Includes
#include <memory>
// Public Includes
#include <blons/graphics/render/renderer.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Wraps a cubemap texture resource with additional format and usage
/// info for easy interaction with the graphics API
////////////////////////////////////////////////////////////////////////////////
class TextureCubemap
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains texture info such as size and type
    ////////////////////////////////////////////////////////////////////////////////
    struct Info
    {
        units::pixel width;  ///< Width of the texture in pixels
        units::pixel height; ///< Height of the texture in pixels
        TextureType type;    ///< Texture format and behaviour info
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new cubemap texture from raw pixel data. Will throw on
    /// failure
    ///
    /// \param pixels Pixel buffer and format info
    ////////////////////////////////////////////////////////////////////////////////
    TextureCubemap(const PixelDataCubemap& pixels);
    ~TextureCubemap() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture::Reload()
    ////////////////////////////////////////////////////////////////////////////////
    void Reload();

    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture::info()
    ////////////////////////////////////////////////////////////////////////////////
    const Info* info() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture::pixels()
    ////////////////////////////////////////////////////////////////////////////////
    const PixelDataCubemap* pixels(bool force_gpu_sync);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture::texture()
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* texture() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture::mutable_texture()
    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<TextureResource> mutable_texture();

private:
    void Init(const PixelDataCubemap& pixels);

    std::unique_ptr<PixelDataCubemap> pixel_data_;
    std::shared_ptr<TextureResource> texture_;
    Info info_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::TextureCubemap
/// \ingroup graphics
///
/// ### Example:
/// \code
/// // Using a cubemap texture is done through initializing from a pixel buffer
/// blons::PixelDataCubemap pixels;
/// pixels.type.format = TextureType::R8G8B8;
/// pixels.type.compression = TextureType::RAW;
/// pixels.type.filter = TextureType::LINEAR;
/// pixels.type.wrap = TextureType::REPEAT;
/// pixels.width = 1;
/// pixels.height = 1;
/// // There are 6 pixel buffers, one for each cube face
/// // Indexed by blons::AxisAlignedNormal
/// for (auto& buffer : pixels.pixels)
/// {
///     buffer.reset(new unsigned char[pixels.width * pixels.height * 3]);
///     buffer.get()[0] = 255;
///     buffer.get()[1] = 255;
///     buffer.get()[2] = 255;
/// }
/// blons::TextureCubemap tex(pixels);
/// shader->SetInput("white_skybox", tex.texture());
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_TEXTURECUBEMAP_H_