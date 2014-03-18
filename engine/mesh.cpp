#include "mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

bool Mesh::Init(const char* filename, bool invert_y)
{
    FILE* file;
    fopen_s(&file, filename, "rb");
    if (file == nullptr)
    {
        return false;
    }

    RawMesh mesh_info;
    size_t vertices_read, uvs_read, normals_read, faces_read;

    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Get the header info
    fread(&mesh_info.vertex_count, sizeof(unsigned int), 1, file);
    fread(&mesh_info.uv_count,     sizeof(unsigned int), 1, file);
    fread(&mesh_info.normal_count, sizeof(unsigned int), 1, file);
    fread(&mesh_info.face_count,   sizeof(unsigned int), 1, file);

    // Needs verts dude + make sure we arent loading some garbage file thatd allocate 4gb of memory
    if (!mesh_info.vertex_count ||
        mesh_info.vertex_count * sizeof(Vector3) > file_size ||
        mesh_info.uv_count * sizeof(Vector2) > file_size ||
        mesh_info.normal_count * sizeof(Vector3) > file_size ||
        mesh_info.face_count * sizeof(unsigned int) > file_size)
    {
        return false;
    }

    // TODO: support non-textured objects (need shader layouts, settings n shit)
    if (!mesh_info.uv_count)
    {
        return false;
    }

    // The seems like the most efficient way to read a lot of data straight into a vector...
    // Kinda gross really
    mesh_info.vertices.resize(mesh_info.vertex_count);
    vertices_read = fread(mesh_info.vertices.data(), sizeof(Vector3), mesh_info.vertex_count, file);

    mesh_info.uvs.resize(mesh_info.uv_count);
    uvs_read = fread(mesh_info.uvs.data(), sizeof(Vector2), mesh_info.uv_count, file);

    mesh_info.normals.resize(mesh_info.normal_count);
    normals_read = fread(mesh_info.normals.data(), sizeof(Vector3), mesh_info.normal_count, file);

    // Allocate 3 uints per face for each type of mesh data supplied
    unsigned int face_size = 3 * (mesh_info.vertex_count > 0) +
                             3 * (mesh_info.uv_count > 0) +
                             3 * (mesh_info.normal_count > 0);
    mesh_info.faces = new unsigned int [mesh_info.face_count * face_size];
    faces_read = fread(mesh_info.faces, sizeof(unsigned int)*face_size, mesh_info.face_count, file);

    // Trigger EOF
    fgetc(file);
    int eof = feof(file);
    fclose(file);

    if (vertices_read != mesh_info.vertex_count || uvs_read != mesh_info.uv_count ||
        normals_read != mesh_info.normal_count || faces_read != mesh_info.face_count || !eof)
    {
        return false;
    }

    vertex_buffer_ = std::unique_ptr<BufferResource>(new BufferResource);
    index_buffer_  = std::unique_ptr<BufferResource>(new BufferResource);

    // 3 vertices for every tri
    vertex_count_ = index_count_ = mesh_info.face_count * 3;

    std::vector<Vertex> vertices;
    vertices.reserve(vertex_count_);

    if (vertices.max_size() < vertex_count_)
    {
        return false;
    }

    std::unique_ptr<unsigned int[]>indices(new unsigned int[index_count_]);
    if (!indices)
    {
        return false;
    }

    std::map<Vertex, unsigned int> vert_lookup;

    // Loads about 20x slower, but +10%~ perf and -50%~ memory
    bool vbo_indexing = false;

    for (int i = 0; i < vertex_count_; i++)
    {
        // TODO: store normals
        // Each face_size is vert*uv*norm*3, we wanna loop once for each vert*uv*norm
        unsigned int face_offset = i * (face_size / 3);

        Vertex new_vert;
        new_vert.pos = mesh_info.vertices[mesh_info.faces[face_offset]  -1];
        new_vert.tex = mesh_info.uvs     [mesh_info.faces[face_offset+1]-1];
        if (invert_y)
        {
            new_vert.tex.y = 1.0f - new_vert.tex.y;
        }

        if (vbo_indexing)
        {
            auto index_match = vert_lookup.find(new_vert);
            if (index_match != vert_lookup.end())
            {
                indices[i] = index_match->second;
            }
            else
            {
                vertices.push_back(new_vert);
                indices[i] = vertices.size() - 1;
                vert_lookup[new_vert] = indices[i];
            }
        }
        else
        {
            vertices.push_back(new_vert);
            indices[i] = vertices.size()-1;
        }
    }
    // Update vertex count to account for removed duplicates
    vertex_count_ = vertices.size();

    if (!g_render->RegisterMesh(vertex_buffer_.get(), index_buffer_.get(), vertices.data(), vertex_count_,
                                indices.get(), index_count_))
    {
        return false;
    }

    return true;
}

void Mesh::Finish()
{
    if (index_buffer_)
    {
        g_render->DestroyBufferResource(index_buffer_.release());
    }
    
    if (vertex_buffer_)
    {
        g_render->DestroyBufferResource(vertex_buffer_.release());
    }

    return;
}

BufferResource* Mesh::GetVertexBuffer()
{
    return vertex_buffer_.get();
}

BufferResource* Mesh::GetIndexBuffer()
{
    return index_buffer_.get();
}

int Mesh::GetVertexCount()
{
    return vertex_count_;
}

int Mesh::GetIndexCount()
{
    return index_count_;
}

