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
#include <blons/graphics/render/renderer.h>
#include <blons/graphics/mesh.h>
#include <blons/graphics/texture.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Provides globally cached resource loading
////////////////////////////////////////////////////////////////////////////////
namespace resource
{
struct MeshBuffer
{
    std::shared_ptr<BufferResource> vertex;
    std::shared_ptr<BufferResource> index;
    unsigned int vertex_count;
    unsigned int index_count;
    MeshData data;
    std::vector<Mesh::TextureInfo> texture_list;
};

struct TextureBuffer
{
    std::shared_ptr<TextureResource> texture;
    Texture::Info info;
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Loads a mesh from disk or engine, or from cache if available.
///
/// If the mesh is cached and loaded into the active context, a pointer to
/// its resource buffer is returned. If cached, but in a different context, a
/// new resource is created and bound to the context. If uncached the file is
/// loaded from disk and bound to the context
///
/// List of valid engine meshes include:
/// * `blons:sphere~radius` Sphere mesh
///
/// \param filename Filename of the mesh to load
/// \return MeshBuffer containing shared BufferResource pointers that will be
/// nullptr on failure, as well as mesh information
////////////////////////////////////////////////////////////////////////////////
MeshBuffer LoadMesh(const std::string& filename);
////////////////////////////////////////////////////////////////////////////////
/// \brief Loads a texture from disk or engine, or from cache if available.
///
/// If the texture is cached and loaded into the active context, a pointer to
/// its resource buffer is returned. If cached, but in a different context, a
/// new resource is created and bound to the context. If uncached the file is
/// loaded from disk and bound to the context
///
/// List of valid engine textures include:
/// * `blons:none` Error colour texture
/// * `blons:normal` Plain normal map
///
/// \param filename Filename of the texture to load
/// \param type Texture parameters to bind
/// \return TextureBuffer containing shared TextureResource pointer that will be
/// nullptr on failure, as well as texture information
////////////////////////////////////////////////////////////////////////////////
TextureBuffer LoadTexture(const std::string& filename, TextureType::Options options);

////////////////////////////////////////////////////////////////////////////////
/// \brief Clears all cached resource buffers, but not cached resource data
////////////////////////////////////////////////////////////////////////////////
void ClearBufferCache();
////////////////////////////////////////////////////////////////////////////////
/// \brief Clears all cached resource buffers and data
////////////////////////////////////////////////////////////////////////////////
void ClearDataCache();
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
