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

#ifndef BLONSTECH_GRAPHICS_MESHIMPORTER_H_
#define BLONSTECH_GRAPHICS_MESHIMPORTER_H_

// Includes
#include <string>
#include <vector>
// Public Includes
#include <blons/graphics/mesh.h>
#include <blons/graphics/texture.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Parses blonsmesh (.bms) files and stores the raw vertex data as well
/// as material info like texture filenames
////////////////////////////////////////////////////////////////////////////////
class MeshImporter
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes and parses a mesh file. Will throw on failure
    ///
    /// \param filename Name of the mesh file to load
    /// \param invert_y If true will vertically flip texture UV coordinates
    ////////////////////////////////////////////////////////////////////////////////
    MeshImporter(std::string filename, bool invert_y);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls MeshImporter(std::string, bool) with an invert_y of **false**
    ////////////////////////////////////////////////////////////////////////////////
    MeshImporter(std::string filename)
        : MeshImporter(filename, false) {}
    ~MeshImporter() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the number of vertices in the loaded mesh
    ///
    /// \return %Vertex count
    ////////////////////////////////////////////////////////////////////////////////
    unsigned int vertex_count() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the number of indices in the loaded mesh
    ///
    /// \return Index count
    ////////////////////////////////////////////////////////////////////////////////
    unsigned int index_count() const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a reference to the mesh data loaded by this importer. Note
    /// this data is only valid for the lifespan of this class
    ///
    /// \return %Mesh data
    ////////////////////////////////////////////////////////////////////////////////
    const MeshData& mesh_data() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a reference to the texture list loaded by this importer.
    /// Note this data is only valid for the lifespan of this class
    ///
    /// \return Texture list
    ////////////////////////////////////////////////////////////////////////////////
    const std::vector<Mesh::TextureInfo>& textures() const;

private:
    MeshData mesh_data_;
    std::vector<Mesh::TextureInfo> textures_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::MeshImporter
/// \ingroup graphics
///
/// ### Example:
/// \code
/// // Creating a new mesh with data from a MeshImporter
/// MeshImporter importer("mesh.bms");
///
/// Mesh mesh(importer.mesh_data(), context);
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_MESHIMPORTER_H_