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

#include <blons/graphics/meshimporter.h>

// Includes
#include <map>

namespace blons
{
MeshImporter::MeshImporter(std::string filename, bool invert_y)
{
    FILE* file;
    unsigned int vertex_count = 0;
    unsigned int index_count = 0;
    unsigned int texture_count = 0;

    fopen_s(&file, filename.c_str(), "rb");
    if (file == nullptr)
    {
        throw "Could not find mesh file";
    }

    fseek(file, 0, SEEK_END);
    std::size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Get the header info
    fread(&vertex_count,  sizeof(unsigned int), 1, file);
    fread(&index_count,   sizeof(unsigned int), 1, file);
    fread(&texture_count, sizeof(unsigned int), 1, file);

    // Needs verts dude + make sure we arent loading some garbage file thatd allocate 4gb of memory
    if (!vertex_count ||
        vertex_count * sizeof(Vector3) > file_size ||
        !index_count ||
        index_count * sizeof(unsigned int) > file_size)
    {
        throw "Corrupted mesh file";
    }

    // The seems like the most efficient way to read a lot of data straight into a vector...
    // Kinda gross really
    mesh_data_.vertices.resize(vertex_count);
    std::size_t large_vertices_read = fread(mesh_data_.vertices.data(), sizeof(Vertex), vertex_count, file);

    mesh_data_.indices.resize(index_count);
    std::size_t large_indices_read = fread(mesh_data_.indices.data(), sizeof(unsigned int), index_count, file);

    // Make sure we can safely convert our mesh data to 32-bit
    if (large_vertices_read >= ULONG_MAX ||
        large_indices_read  >= ULONG_MAX)
    {
        throw "Too much mesh data!";
    }
    auto vertices_read = static_cast<unsigned int>(large_vertices_read);
    auto indices_read = static_cast<unsigned int>(large_indices_read);

    // Load in texture data
    for (unsigned int i = 0; i < texture_count; i++)
    {
        Mesh::TextureInfo tex;
        unsigned int tex_string_len;
        fread(&tex.type, sizeof(unsigned int), 1, file);
        fread(&tex_string_len, sizeof(unsigned int), 1, file);
        for (unsigned int j = 0; j < tex_string_len; j++)
        {
            tex.filename += fgetc(file);
        }
        textures_.push_back(tex);
    }

    // Trigger EOF
    fgetc(file);
    int eof = feof(file);
    fclose(file);

    if (vertices_read != vertex_count || indices_read != index_count || !eof)
    {
        throw "Could not read mesh file successfully";
    }

    if (invert_y)
    {
        for (auto& v : mesh_data_.vertices)
        {
            v.tex.y = 1.0f - v.tex.y;
        }
    }
}

unsigned int MeshImporter::vertex_count() const
{
    std::size_t vertex_count = mesh_data_.vertices.size();
    return static_cast<unsigned int>(vertex_count);
}

unsigned int MeshImporter::index_count() const
{
    std::size_t index_count = mesh_data_.indices.size();
    return static_cast<unsigned int>(index_count);
}

const MeshData& MeshImporter::mesh_data() const
{
    return mesh_data_;
}

const std::vector<Mesh::TextureInfo>& MeshImporter::textures() const
{
    return textures_;
}
} // namespace blons
