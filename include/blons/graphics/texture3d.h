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

#ifndef BLONSTECH_GRAPHICS_TEXTURE3D_H_
#define BLONSTECH_GRAPHICS_TEXTURE3D_H_

// Includes
#include <memory>
// Public Includes
#include <blons/graphics/texture.h>
#include <blons/graphics/render/renderer.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Wraps a 3D texture resource with additional format and usage info for
/// easy interaction with the graphics API
////////////////////////////////////////////////////////////////////////////////
class Texture3D
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains texture info such as size and type
    ////////////////////////////////////////////////////////////////////////////////
    struct Info
    {
        units::pixel width;  ///< Width of the texture in pixels
        units::pixel height; ///< Height of the texture in pixels
        units::pixel depth;  ///< Depth of the texture in pixels
        TextureType type;    ///< Texture format and behaviour info
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new 3D texture from raw pixel data. Will throw on failure
    ///
    /// \param pixels Pixel buffer and format info
    ////////////////////////////////////////////////////////////////////////////////
    Texture3D(const PixelData3D& pixels);
    ~Texture3D() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture::Reload()
    ////////////////////////////////////////////////////////////////////////////////
    void Reload();

    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture::info()
    ////////////////////////////////////////////////////////////////////////////////
    Info info() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture::pixels()
    ////////////////////////////////////////////////////////////////////////////////
    const PixelData3D* pixels(bool force_gpu_sync);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture::texture()
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* texture() const;

private:
    void Init(const PixelData3D& pixels);

    std::unique_ptr<PixelData3D> pixel_data_;
    std::shared_ptr<TextureResource> texture_;
    Info info_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Texture3D
/// \ingroup graphics
///
/// ### Example:
/// \code
/// // Using a 3D texture is done through initializing from a pixel buffer
/// blons::PixelData3D pixels;
/// pixels.type.format = TextureType::R8G8B8;
/// pixels.type.compression = TextureType::RAW;
/// pixels.type.filter = TextureType::LINEAR;
/// pixels.type.wrap = TextureType::REPEAT;
/// pixels.width = 1;
/// pixels.height = 1;
/// pixels.depth = 1;
/// pixels.pixels.reset(new unsigned char[pixels.width * pixels.height * pixels.depth * 3]);
/// pixels.pixels.get()[0] = 255;
/// pixels.pixels.get()[1] = 255;
/// pixels.pixels.get()[2] = 255;
/// blons::Texture tex(pixels);
/// shader->SetInput("white_cube", tex.texture());
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_TEXTURE3D_H_