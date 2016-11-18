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

#ifndef BLONSTECH_GRAPHICS_TEXTURE_H_
#define BLONSTECH_GRAPHICS_TEXTURE_H_

// Includes
#include <memory>
// Public Includes
#include <blons/graphics/render/renderer.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Wraps a texture resource with additional format and usage info for
/// easy interaction with the graphics API
////////////////////////////////////////////////////////////////////////////////
class Texture
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
    /// \brief Initializes a new texture from an image file. Will throw on failure
    ///
    /// \param filename Image file on disk
    /// \param options Texture filtering and wrapping options
    ////////////////////////////////////////////////////////////////////////////////
    Texture(std::string filename, TextureType::Options options);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new texture from raw pixel data. Will throw on failure
    ///
    /// \param pixels Pixel buffer and format info
    ////////////////////////////////////////////////////////////////////////////////
    Texture(const PixelData& pixels);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Texture(const PixelData&)
    ////////////////////////////////////////////////////////////////////////////////
    Texture(const PixelData3D& pixels);
    ~Texture() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads the texture to be attached to the active rendering context
    ///
    /// \return True if reinitialized successfully
    ////////////////////////////////////////////////////////////////////////////////
    bool Reload();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves texture information like dimensions & usage type
    ///
    /// \return Info struct containing texture info
    ////////////////////////////////////////////////////////////////////////////////
    Info info() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a pointer to the texture resource for rendering
    ///
    /// \return Texture resource reference
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* texture() const;

private:
    bool Init(std::string filename, TextureType::Options options);
    bool Init(const PixelData& pixels);
    bool Init(const PixelData3D& pixels);

    // Empty if initialized from PixelData or voxel data
    std::string filename_;
    // nullptr if initialized from filename or voxel data
    std::unique_ptr<PixelData> pixel_data_;
    // nullptr if initialized from filename or pixel data
    std::unique_ptr<PixelData3D> pixel_data_3d_;

    std::shared_ptr<TextureResource> texture_;
    Info info_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Texture
/// \ingroup graphics
///
/// ### Example:
/// \code
/// // Using a texture is as simple as pushing it to a shader
/// // Though sprites & models are almost always more suitable
/// blons::Texture tex("sprite.png", blons::Texture::SPRITE);
/// shader->SetInput("sprite", tex.texture());
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_TEXTURE_H_