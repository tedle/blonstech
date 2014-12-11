#include <blons/graphics/mesh.h>

// Public Includes
#include <blons/graphics/meshimporter.h>

namespace blons
{
Mesh::Mesh(const MeshImporter& mesh, RenderContext& context)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
    index_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
    mesh_data_.vertices = mesh.mesh_data()->vertices;
    mesh_data_.indices = mesh.mesh_data()->indices;

    if (!context->Register3DMesh(vertex_buffer_.get(), index_buffer_.get(),
                                 mesh_data_.vertices.data(), vertex_count(),
                                 mesh_data_.indices.data(), index_count()))
    {
        throw "Failed to register mesh data";
    }
}

Mesh::~Mesh()
{
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
    if (vertex_count >= ULONG_MAX)
    {
        throw "Too many vertices!";
    }
    return static_cast<unsigned int>(vertex_count);
}

unsigned int Mesh::index_count() const
{
    std::size_t index_count = mesh_data_.indices.size();
    if (index_count >= ULONG_MAX)
    {
        throw "Too many indices!";
    }
    return static_cast<unsigned int>(index_count);
}
} // namespace blons
