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
// TODO: Refactor this with some helper functions
MeshImporter::MeshImporter(std::string filename, bool invert_y)
{
    uv_count_ = 0;
    normal_count_ = 0;
    face_count_ = 0;

    FILE* file;
    std::vector<Vector3> vertices, normals;
    std::vector<Vector2> uvs;
    std::vector<unsigned int> faces;
    unsigned int vertex_count = 0;
    unsigned int index_count = 0;

    fopen_s(&file, filename.c_str(), "rb");
    if (file == nullptr)
    {
        throw "Could not find mesh file";
    }

    unsigned int vertices_read, uvs_read, normals_read, faces_read, texture_count;

    fseek(file, 0, SEEK_END);
    std::size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Get the header info
    fread(&vertex_count,  sizeof(unsigned int), 1, file);
    fread(&uv_count_,     sizeof(unsigned int), 1, file);
    fread(&normal_count_, sizeof(unsigned int), 1, file);
    fread(&face_count_,   sizeof(unsigned int), 1, file);
    fread(&texture_count, sizeof(unsigned int), 1, file);

    // Needs verts dude + make sure we arent loading some garbage file thatd allocate 4gb of memory
    if (!vertex_count ||
        vertex_count * sizeof(Vector3) > file_size ||
        uv_count_ * sizeof(Vector2) > file_size ||
        normal_count_ * sizeof(Vector3) > file_size ||
        face_count_ * sizeof(unsigned int) > file_size)
    {
        throw "Corrupted mesh file";
    }

    // TODO: support non-textured objects (need shader layouts, settings n shit)
    if (!uv_count_)
    {
        throw "Mesh file needs UV data";
    }

    // The seems like the most efficient way to read a lot of data straight into a vector...
    // Kinda gross really
    vertices.resize(vertex_count);
    std::size_t large_vertices_read = fread(vertices.data(), sizeof(Vector3), vertex_count, file);

    uvs.resize(uv_count_);
    std::size_t large_uvs_read = fread(uvs.data(), sizeof(Vector2), uv_count_, file);

    normals.resize(normal_count_);
    std::size_t large_normals_read = fread(normals.data(), sizeof(Vector3), normal_count_, file);

    // Allocate 3 uints per face for each type of mesh data supplied
    unsigned int face_size = 3 * (vertex_count > 0) +
                             3 * (uv_count_ > 0) +
                             3 * (normal_count_ > 0);
    faces.resize(face_count_ * face_size);
    std::size_t large_faces_read = fread(faces.data(), sizeof(unsigned int)*face_size, face_count_, file);

    // Make sure we can safely convert our mesh data to 32-bit
    if (large_vertices_read >= ULONG_MAX ||
        large_uvs_read      >= ULONG_MAX ||
        large_normals_read  >= ULONG_MAX ||
        large_faces_read    >= ULONG_MAX)
    {
        throw "Too much mesh data!";
    }
    vertices_read = static_cast<unsigned int>(large_vertices_read);
    uvs_read = static_cast<unsigned int>(large_uvs_read);
    normals_read = static_cast<unsigned int>(large_normals_read);
    faces_read = static_cast<unsigned int>(large_faces_read);

    // Load in texture data
    for (unsigned int i = 0; i < texture_count; i++)
    {
        TextureInfo tex;
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

    if (vertices_read != vertex_count || uvs_read != uv_count_ ||
        normals_read != normal_count_ || faces_read != face_count_ || !eof)
    {
        throw "Could not read mesh file successfully";
    }

    // 3 vertices for every tri
    vertex_count = index_count = face_count_ * 3;

    if (vertex_count >= ULONG_MAX)
    {
        throw "Too many vertices!";
    }
    mesh_data_.vertices.reserve(vertex_count);

    if (mesh_data_.vertices.max_size() < vertex_count)
    {
        throw "Could not allocate memory for new mesh";
    }

    if (index_count >= ULONG_MAX)
    {
        throw "Too many indices!";
    }
    mesh_data_.indices.resize(index_count);

    // If there's no UVs, normals immediately follow vertex
    const int norm_offset = (uv_count_ > 0 ? 2 : 1);
    // In case we need to manually calculate normals, need some way
    // to cache normal value as its updated once every 3 iterations (once per tri)
    Vector3 current_normal(0.0, 0.0, 0.0);
    Vector3 current_tangent(0.0, 0.0, 0.0);
    Vector3 current_bitangent(0.0, 0.0, 0.0);

    // If true: loads about 2x slower, but +10%~ perf and -50%~ memory
    const bool vbo_indexing = false;
    std::map<Vertex, unsigned int> vert_lookup;

    for (unsigned int i = 0; i < vertex_count; i++)
    {
        // TODO: store normals
        // Each face_size is vert*uv*norm*3, we wanna loop once for each vert*uv*norm
        unsigned int face_offset = i * (face_size / 3);

        Vertex new_vert;
        // Get vertex data
        new_vert.pos = vertices[faces[face_offset]  -1];
        // Get and correct UV data
        new_vert.tex = uvs     [faces[face_offset+1]-1];
        if (invert_y)
        {
            new_vert.tex.y = 1.0f - new_vert.tex.y;
        }
        // Get normal data
        if (normal_count_ > 0)
        {
            current_normal = normals[faces[face_offset+norm_offset]-1];
        }
        // Normal data isn't baked in, calculate defaults
        // We only do this once per tri since we need 3 vertices to calculate
        else if (i % 3 == 0)
        {
            Vector3 v1, v2, v3;
            size_t face_index_size = face_size / 3;
            v1 = vertices[faces[face_offset+(face_index_size*0)]-1];
            v2 = vertices[faces[face_offset+(face_index_size*1)]-1];
            v3 = vertices[faces[face_offset+(face_index_size*2)]-1];

            current_normal = Vector3Cross(v2 - v1, v3 - v1);
            current_normal = Vector3Normalize(current_normal);
        }
        new_vert.norm = current_normal;

        // Calculate tangent space matrix components
        // TODO: Tangent smoothing for when vbo indexing is disabled
        if (i % 3 == 0)
        {
            Vector3 v1, v2, v3;
            size_t face_index_size = face_size / 3;
            v1 = vertices[faces[face_offset+(face_index_size*0)]-1];
            v2 = vertices[faces[face_offset+(face_index_size*1)]-1];
            v3 = vertices[faces[face_offset+(face_index_size*2)]-1];

            Vector2 uv1, uv2, uv3;
            if (uv_count_ > 0)
            {
                uv1 = uvs[faces[face_offset + 1 + (face_index_size * 0)] - 1];
                uv2 = uvs[faces[face_offset + 1 + (face_index_size * 1)] - 1];
                uv3 = uvs[faces[face_offset + 1 + (face_index_size * 2)] - 1];
            }

            // q  = position vectors from origin
            // st = uv vectors from origin
            // Solve for T & B
            // [q1.xyz]   [st1.uv][T.xyz]
            // [      ] = [      ][     ]
            // [q2.xyz]   [st2.uv][B.xyz]
            // ---> multiply by inverse of st matrix
            // [T.xyz]               1             [st2.v, -st1.v][q1.xyz]
            // [     ] = ------------------------- [             ][      ]
            // [B.xyz]   st1.u*st2.v - st2.u*st1.v [-st2.u, st1.u][q2.xyz]

            // v1 as origin
            Vector3 q1, q2;
            q1 = v2 - v1;
            q2 = v3 - v1;

            // uv1 as origin
            Vector2 st1, st2;
            st1 = uv2 - uv1;
            st2 = uv3 - uv1;

            // Reciprocal used to scale the inverse st matrix properly
            float k = st1.x * st2.y - st2.x * st1.y;

            // Tangents & bitangents are based off UVs, if we get invalid UVs or they don't exist...
            // We build vectors to make up an identity matrix instead
            if (k == 0)
            {
                current_tangent = Vector3(1.0, 0.0, 0.0);
                current_bitangent = Vector3(0.0, 1.0, 0.0);
            }
            else
            {
                // Coefficient for the inverse of the st matrix
                float inv_st = 1.0f / k;
                // Direction of s vector
                Vector3 s_dir = ((q1 * st2.y - q2 * st1.y) * inv_st);
                // Direction of t vector
                Vector3 t_dir = ((q2 * st1.x - q1 * st2.x) * inv_st);

                // Orthogonalize tangent (s_dir)
                current_tangent = (s_dir - current_normal * Vector3Dot(current_normal, s_dir));
                current_bitangent = t_dir;

                // Ensure right handedness
                // Have to invert comparison for some reason I don't understand
                // "Correct" comparison causes certain X normals to invert
                if (Vector3Dot(Vector3Cross(current_normal, current_tangent), t_dir) > 0.0f)
                {
                    current_tangent *= -1.0f;
                }
            }
        }
        new_vert.tan = current_tangent;
        new_vert.bitan = current_bitangent;

        if (vbo_indexing)
        {
            auto& index_match = vert_lookup.find(new_vert);
            if (index_match != vert_lookup.end())
            {
                mesh_data_.indices[i] = index_match->second;
                // Tangent smoothing (have to normalize at the end of loop)
                mesh_data_.vertices[index_match->second].tan += new_vert.tan;
                mesh_data_.vertices[index_match->second].bitan += new_vert.bitan;
            }
            else
            {
                mesh_data_.vertices.push_back(new_vert);
                mesh_data_.indices[i] = static_cast<unsigned int>(mesh_data_.vertices.size()) - 1;
                vert_lookup[new_vert] = mesh_data_.indices[i];
            }
        }
        else
        {
            mesh_data_.vertices.push_back(new_vert);
            mesh_data_.indices[i] = static_cast<unsigned int>(mesh_data_.vertices.size()) - 1;
        }
    }
    // Normalize tangents
    for (auto& v : mesh_data_.vertices)
    {
        v.tan = Vector3Normalize(v.tan);
        v.bitan = Vector3Normalize(v.bitan);
    }
    // Update vertex count to account for removed duplicates
    log::Debug("%.1f%%v", (((float)vertex_count - (float)mesh_data_.vertices.size()) / (float)vertex_count) * 100.0);
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

unsigned int MeshImporter::uv_count() const
{
    return uv_count_;
}

unsigned int MeshImporter::normal_count() const
{
    return normal_count_;
}

unsigned int MeshImporter::face_count() const
{
    return face_count_;
}

const MeshData& MeshImporter::mesh_data() const
{
    return mesh_data_;
}

const std::vector<MeshImporter::TextureInfo>& MeshImporter::textures() const
{
    return textures_;
}
} // namespace blons
