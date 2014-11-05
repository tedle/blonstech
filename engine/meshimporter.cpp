#include "meshimporter.h"

// Includes
#include <map>

MeshImporter::MeshImporter()
{
    vertex_count_ = 0;
    index_count_ = 0;
    uv_count_ = 0;
    normal_count_ = 0;
    face_count_ = 0;
}

MeshImporter::~MeshImporter()
{

}

bool MeshImporter::Load(const char* filename)
{
    return Load(filename, false);
}

bool MeshImporter::Load(const char* filename, bool invert_y)
{
    FILE* file;
    std::vector<Vector3> vertices, normals;
    std::vector<Vector2> uvs;
    std::vector<unsigned int> faces;

    fopen_s(&file, filename, "rb");
    if (file == nullptr)
    {
        return false;
    }

    size_t vertices_read, uvs_read, normals_read, faces_read, texture_count;

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Get the header info
    fread(&vertex_count_, sizeof(unsigned int), 1, file);
    fread(&uv_count_,     sizeof(unsigned int), 1, file);
    fread(&normal_count_, sizeof(unsigned int), 1, file);
    fread(&face_count_,   sizeof(unsigned int), 1, file);
    fread(&texture_count, sizeof(unsigned int), 1, file);

    // Needs verts dude + make sure we arent loading some garbage file thatd allocate 4gb of memory
    if (!vertex_count_ ||
        vertex_count_ * sizeof(Vector3) > file_size ||
        uv_count_ * sizeof(Vector2) > file_size ||
        normal_count_ * sizeof(Vector3) > file_size ||
        face_count_ * sizeof(unsigned int) > file_size)
    {
        return false;
    }

    // TODO: support non-textured objects (need shader layouts, settings n shit)
    if (!uv_count_)
    {
        return false;
    }

    // The seems like the most efficient way to read a lot of data straight into a vector...
    // Kinda gross really
    vertices.resize(vertex_count_);
    vertices_read = fread(vertices.data(), sizeof(Vector3), vertex_count_, file);

    uvs.resize(uv_count_);
    uvs_read = fread(uvs.data(), sizeof(Vector2), uv_count_, file);

    normals.resize(normal_count_);
    normals_read = fread(normals.data(), sizeof(Vector3), normal_count_, file);

    // Allocate 3 uints per face for each type of mesh data supplied
    unsigned int face_size = 3 * (vertex_count_ > 0) +
                             3 * (uv_count_ > 0) +
                             3 * (normal_count_ > 0);
    faces.resize(face_count_ * face_size);
    faces_read = fread(faces.data(), sizeof(unsigned int)*face_size, face_count_, file);

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

    if (vertices_read != vertex_count_ || uvs_read != uv_count_ ||
        normals_read != normal_count_ || faces_read != face_count_ || !eof)
    {
        return false;
    }

    // 3 vertices for every tri
    vertex_count_ = index_count_ = face_count_ * 3;

    vertices_.reserve(vertex_count_);

    if (vertices_.max_size() < vertex_count_)
    {
        return false;
    }

    indices_.resize(index_count_);

    // If there's no UVs, normals immediately follow vertex
    const int norm_offset = (uv_count_ > 0 ? 2 : 1);
    // In case we need to manually calculate normals, need some way
    // to cache normal value as its updated once every 3 iterations (once per tri)
    Vector3 current_normal(0.0, 0.0, 0.0);

    // If true: loads about 2x slower, but +10%~ perf and -50%~ memory
    const bool vbo_indexing = false;
    std::map<Vertex, unsigned int> vert_lookup;

    for (unsigned int i = 0; i < vertex_count_; i++)
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
            // Normals range from -1, 1 be we need to store as 0, 1
            current_normal.x = (current_normal.x + 1) / 2;
            current_normal.y = (current_normal.y + 1) / 2;
            current_normal.z = (current_normal.z + 1) / 2;
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
            // (v2-v1) * (v3-v1)
            Vector3 v21, v31;
            v21 = v2 - v1;
            v31 = v3 - v1;
            current_normal = Vector3Cross(v21, v31);
            // Normal x+y+z must be 1
            current_normal = Vector3Normalize(current_normal);
            // Normals range from -1, 1 but we need to store as 0, 1
            current_normal.x = (current_normal.x + 1) / 2;
            current_normal.y = (current_normal.y + 1) / 2;
            current_normal.z = (current_normal.z + 1) / 2;
        }
        new_vert.norm = current_normal;


        if (vbo_indexing)
        {
            auto index_match = vert_lookup.find(new_vert);
            if (index_match != vert_lookup.end())
            {
                indices_[i] = index_match->second;
            }
            else
            {
                vertices_.push_back(new_vert);
                indices_[i] = vertices_.size() - 1;
                vert_lookup[new_vert] = indices_[i];
            }
        }
        else
        {
            vertices_.push_back(new_vert);
            indices_[i] = vertices_.size()-1;
        }
    }
    // Update vertex count to account for removed duplicates
    g_log->Debug("%.1f%%v", (((float)vertex_count_ - (float)vertices_.size()) / (float)vertex_count_) * 100.0);
    vertex_count_ = vertices_.size();

    return true;
}

unsigned int MeshImporter::vertex_count()
{
    return vertex_count_;
}

unsigned int MeshImporter::index_count()
{
    return index_count_;
}

unsigned int MeshImporter::uv_count()
{
    return uv_count_;
}

unsigned int MeshImporter::normal_count()
{
    return normal_count_;
}

unsigned int MeshImporter::face_count()
{
    return face_count_;
}

std::vector<Vertex>& MeshImporter::vertices()
{
    return vertices_;
}

std::vector<unsigned int>& MeshImporter::indices()
{
    return indices_;
}

std::vector<MeshImporter::TextureInfo>& MeshImporter::textures()
{
    return textures_;
}