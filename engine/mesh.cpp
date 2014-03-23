#include "mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

bool Mesh::Init(MeshImporter* mesh_data)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(new BufferResource);
    index_buffer_  = std::unique_ptr<BufferResource>(new BufferResource);
    vertex_count_ = mesh_data->vertex_count();
    index_count_ = mesh_data->index_count();

    if (!g_render->RegisterMesh(vertex_buffer_.get(), index_buffer_.get(), mesh_data->vertices().data(), vertex_count_,
                                mesh_data->indices().data(), index_count_))
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

