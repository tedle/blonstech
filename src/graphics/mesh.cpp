#include <blons/graphics/mesh.h>

// Public Includes
#include <blons/graphics/meshimporter.h>

namespace blons
{
Mesh::Mesh(const MeshData& mesh_data, RenderContext& context)
{
    vertex_buffer_.reset(context->MakeBufferResource());
    index_buffer_.reset(context->MakeBufferResource());
    mesh_data_.vertices = mesh_data.vertices;
    mesh_data_.indices = mesh_data.indices;

    // Graphics APIs dont support having more than 4 billion vertices...
    // I'm OK with that
    if (mesh_data_.vertices.size() >= ULONG_MAX)
    {
        throw "Too many vertices!";
    }

    if (mesh_data_.indices.size() >= ULONG_MAX)
    {
        throw "Too many indices!";
    }

    if (!context->Register3DMesh(vertex_buffer_.get(), index_buffer_.get(),
                                 mesh_data_.vertices.data(), vertex_count(),
                                 mesh_data_.indices.data(), index_count()))
    {
        throw "Failed to register mesh data";
    }
}

BufferResource* Mesh::vertex_buffer() const
{
    return vertex_buffer_.get();
}

BufferResource* Mesh::index_buffer() const
{
    return index_buffer_.get();
}

unsigned int Mesh::vertex_count() const
{
    std::size_t vertex_count = mesh_data_.vertices.size();
    return static_cast<unsigned int>(vertex_count);
}

unsigned int Mesh::index_count() const
{
    std::size_t index_count = mesh_data_.indices.size();
    return static_cast<unsigned int>(index_count);
}
} // namespace blons
