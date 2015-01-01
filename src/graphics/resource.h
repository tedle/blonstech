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

#ifndef BLONSTECH_GRAPHICS_RESOURCE_H_
#define BLONSTECH_GRAPHICS_RESOURCE_H_

// Public Includes
#include <blons/graphics/render/render.h>
#include <blons/graphics/texture.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Provides globally cached resource loading
////////////////////////////////////////////////////////////////////////////////
namespace resource
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Loads a texture from disk or engine, or from cache if available.
///
/// If the texture is cached and loaded into the supplied context, a pointer to
/// its resource buffer is returned. If cached, but in a different context, a
/// new resource is created and bound to the context. If uncached the file is
/// loaded from disk and bound to the context
///
/// List of valid engine textures include:
/// * `blons:none` Error diffuse texture
/// * `blons:normal` Plain normal map
///
/// \param filename Filename of the texture to load
/// \param type Texture parameters to bind
/// \param[out] info Texture info like width, height, and type
/// \param context Handle to the current rendering context
/// \return Shared TextureResource pointer, or nullptr on failure
////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<TextureResource> LoadTexture(const std::string& filename, Texture::Type type, Texture::Info* info, RenderContext& context);

////////////////////////////////////////////////////////////////////////////////
/// \brief Clears all cached resource buffers, but not cached resource data
////////////////////////////////////////////////////////////////////////////////
void ClearBufferCache();
} // namespace resource
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \namespace blons::resource
/// \ingroup graphics
///
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_RESOURCE_H_
