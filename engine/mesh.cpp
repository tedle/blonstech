#include "mesh.h"

// Local Includes
#include "meshimporter.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

bool Mesh::Load(MeshImporter* mesh_data, RenderContext& context)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    index_buffer_  = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    vertex_count_ = mesh_data->vertex_count();
    index_count_ = mesh_data->index_count();

    if (!context->RegisterMesh(vertex_buffer_.get(), index_buffer_.get(), mesh_data->vertices().data(), vertex_count_,
                                mesh_data->indices().data(), index_count_))
    {
        return false;
    }

    return true;
}

void Mesh::Finish(RenderContext& context)
{
    if (index_buffer_)
    {
        context->DestroyBufferResource(index_buffer_.release());
    }
    
    if (vertex_buffer_)
    {
        context->DestroyBufferResource(vertex_buffer_.release());
    }

    return;
}

BufferResource* Mesh::vertex_buffer()
{
    return vertex_buffer_.get();
}

BufferResource* Mesh::index_buffer()
{
    return index_buffer_.get();
}

int Mesh::vertex_count()
{
    return vertex_count_;
}

int Mesh::index_count()
{
    return index_count_;
}

