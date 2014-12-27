////////////////////////////////////////////////////////////////////////////////
/// blonstech
/// Copyright(c) 2014 Dominic Bowden
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files(the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef BLONSTECH_GRAPHICS_MESH_H_
#define BLONSTECH_GRAPHICS_MESH_H_

// Includes
#include <memory>
// Public Includes
#include <blons/graphics/render/render.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Stores the vertices and indices of a mesh
////////////////////////////////////////////////////////////////////////////////
struct MeshData
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Vector of vertices
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<Vertex> vertices;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Vector of indices
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<unsigned int> indices;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Stores the render buffers of a mesh as well as its vertex data
////////////////////////////////////////////////////////////////////////////////
class Mesh
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief initializes a mesh with the supplied mesh data
    ///
    /// \param mesh_data %Vertex and index data to bind to the mesh buffers
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Mesh(const MeshData& mesh_data, RenderContext& context);
    ~Mesh() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the mesh's vertex buffer resource
    ///
    /// \return Vertex buffer
    ////////////////////////////////////////////////////////////////////////////////
    BufferResource* vertex_buffer() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the mesh's index buffer resource
    ///
    /// \return Index buffer
    ////////////////////////////////////////////////////////////////////////////////
    BufferResource* index_buffer() const;
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

private:
    std::unique_ptr<BufferResource> vertex_buffer_, index_buffer_;
    MeshData mesh_data_;
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
/// Mesh mesh(importer.mesh_data(), context);
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_MESH_H_
