#include "graphics/render/drawbatcher.h"

// Includes
#include <algorithm>

namespace blons
{
DrawBatcher::DrawBatcher(RenderContext& context)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
    index_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
    context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, 0, nullptr, 0);

    array_size_ = 1;
    vertices_ = std::unique_ptr<Vertex>(new Vertex[array_size_]);
    indices_ = std::unique_ptr<unsigned int>(new unsigned int[array_size_]);

    vertex_count_ = 0;
    index_count_ = 0;
    vertex_idx_ = 0;
    index_idx_ = 0;
}

void DrawBatcher::Append(const MeshData& mesh_data)
{
    const unsigned int vert_size = static_cast<unsigned int>(mesh_data.vertices.size());
    const unsigned int index_size = static_cast<unsigned int>(mesh_data.indices.size());

    // Dynamically resize our arrays as needed
    if (vertex_idx_ + vert_size > array_size_ ||
        index_idx_ + index_size > array_size_)
    {
        // TODO: Change this to *= 2? Maybe? Maybe not?
        array_size_ = std::max(vertex_idx_ + vert_size, index_idx_ + index_size);

        auto new_vertices = std::unique_ptr<Vertex>(new Vertex[array_size_]);
        auto new_indices = std::unique_ptr<unsigned int>(new unsigned int[array_size_]);

        memcpy(new_vertices.get(), vertices_.get(), sizeof(Vertex) * vertex_idx_);
        memcpy(new_indices.get(), indices_.get(), sizeof(unsigned int) * index_idx_);

        vertices_ = std::move(new_vertices);
        indices_ = std::move(new_indices);
    }
    // memcpy is noticably faster in debug builds, not so much with compiler optimizations
    memcpy(vertices_.get()+vertex_idx_, mesh_data.vertices.data(), sizeof(Vertex) * vert_size);
    // Caching these helps debug perf
    auto indices_ptr = indices_.get();
    auto batch_indices_ptr = mesh_data.indices.data();
    // Can't memcpy here because indices need to be incremented as meshes are appended
    for (std::size_t j = 0; j < index_size; j++)
    {
        indices_ptr[index_idx_ + j] = batch_indices_ptr[j] + vertex_idx_;
    }
    vertex_idx_ += vert_size;
    index_idx_ += index_size;
}

void DrawBatcher::Render(RenderContext& context)
{
    vertex_count_ = vertex_idx_;
    index_count_ = index_idx_;

    context->SetMeshData(vertex_buffer_.get(), index_buffer_.get(),
                         vertices_.get(), vertex_count_,
                         indices_.get(), index_count_);
    context->BindMeshBuffer(vertex_buffer_.get(), index_buffer_.get());

    vertex_idx_ = 0;
    index_idx_ = 0;
}

int DrawBatcher::index_count() const
{
    return index_count_;
}
} // namespace blons