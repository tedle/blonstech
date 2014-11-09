#include "mesh.h"

// Local Includes
#include "meshimporter.h"

namespace blons
{
Mesh::Mesh(MeshImporter* mesh, RenderContext& context)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    index_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    mesh_data_.vertices = mesh->mesh_data().vertices;
    mesh_data_.indices = mesh->mesh_data().indices;

    if (!context->RegisterMesh(vertex_buffer_.get(), index_buffer_.get(),
                               mesh_data_.vertices.data(), mesh_data_.vertices.size(),
                               mesh_data_.indices.data(), mesh_data_.indices.size()))
    {
        throw "Failed to register mesh data";
    }
}

Mesh::~Mesh()
{
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
    return mesh_data_.vertices.size();
}

int Mesh::index_count()
{
    return mesh_data_.indices.size();
}
} // namespace blons
