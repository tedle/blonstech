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

#ifndef BLONSTECH_GRAPHICS_MESH_H_
#define BLONSTECH_GRAPHICS_MESH_H_

// Includes
#include <memory>
// Public Includes
#include <blons/graphics/render/renderer.h>
#include <blons/graphics/texture.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Stores the render buffers of a mesh as well as its vertex data
////////////////////////////////////////////////////////////////////////////////
class Mesh
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Stores the filename of a texture as well as its purpose (albedo,
    /// normal, etc)
    ////////////////////////////////////////////////////////////////////////////////
    struct TextureInfo
    {
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Location of the texture on disk
        ////////////////////////////////////////////////////////////////////////////////
        std::string filename;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Usage of texture
        ////////////////////////////////////////////////////////////////////////////////
        enum Type {
            ALBEDO, ///< Used to store colour of a mesh
            NORMAL, ///< Used to store normals of a mesh
            LIGHT   ///< Used to store lightmap data of a mesh
        } type; ///< \copybrief TextureInfo::Type
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief initializes a mesh from the supplied file on disk
    ///
    /// \param mesh_filename Name of the blonsmesh file on disk
    ////////////////////////////////////////////////////////////////////////////////
    Mesh(const std::string& mesh_filename);
    ~Mesh() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads the mesh to be attached to the active rendering context
    /// Will throw on failure
    ////////////////////////////////////////////////////////////////////////////////
    void Reload();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the mesh's BufferResource
    ///
    /// \return Mesh buffer
    ////////////////////////////////////////////////////////////////////////////////
    BufferResource* buffer() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the number of vertices in this mesh
    ///
    /// \return Vertex count
    ////////////////////////////////////////////////////////////////////////////////
    unsigned int vertex_count() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the number of indices in this mesh
    ///
    /// \return Index count
    ////////////////////////////////////////////////////////////////////////////////
    unsigned int index_count() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the raw vertex and index data of the mesh
    ///
    /// \return Mesh data
    ////////////////////////////////////////////////////////////////////////////////
    const MeshData& mesh() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a reference to the texture list used by this mesh.
    /// Note this data is only valid for the lifespan of this class
    ///
    /// \return Texture list
    ////////////////////////////////////////////////////////////////////////////////
    const std::vector<Mesh::TextureInfo>& textures() const;

private:
    std::string filename_;

    std::shared_ptr<BufferResource> buffer_;
    unsigned int vertex_count_, index_count_;
    MeshData data_;

    std::vector<Mesh::TextureInfo> texture_list_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Mesh
/// \ingroup graphics
///
/// ### Example:
/// \code
/// // Creating a new mesh from a file
/// MeshImporter importer("mesh.bms");
///
/// Mesh mesh(importer.mesh_data());
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_MESH_H_
