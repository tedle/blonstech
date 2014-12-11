#include <blons/graphics/render/drawbatcher.h>

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
    vertex_count_ = 0;
    index_count_ = 0;
    vertex_idx_ = 0;
    index_idx_ = 0;
}

void DrawBatcher::Append(const MeshData& mesh_data, RenderContext& context)
{
    const unsigned int vert_size = static_cast<unsigned int>(mesh_data.vertices.size());
    const unsigned int index_size = static_cast<unsigned int>(mesh_data.indices.size());

    // Dynamically resize our buffers as needed
    if (vertex_idx_ + vert_size > array_size_ ||
        index_idx_ + index_size > array_size_)
    {
        // TODO: Change this to *= 2? Maybe? Maybe not?
        array_size_ = std::max(vertex_idx_ + vert_size, index_idx_ + index_size);

        vertex_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
        index_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
        context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, array_size_, nullptr, array_size_);
    }
    void* vptr = nullptr;
    void* iptr = nullptr;
    context->MapMeshData(vertex_buffer_.get(), index_buffer_.get(), &vptr, &iptr);
    Vertex* vertices = (Vertex*)vptr;
    unsigned int* indices = (unsigned int*)iptr;

    // memcpy is noticably faster in debug builds, not so much with compiler optimizations
    memcpy(vertices+vertex_idx_, mesh_data.vertices.data(), sizeof(Vertex) * vert_size);
    // Caching this helps debug perf
    auto batch_indices_ptr = mesh_data.indices.data();
    // Can't memcpy here because indices need to be incremented as meshes are appended
    for (std::size_t j = 0; j < index_size; j++)
    {
        indices[index_idx_ + j] = batch_indices_ptr[j] + vertex_idx_;
    }

    vertex_idx_ += vert_size;
    index_idx_ += index_size;
}

void DrawBatcher::Render(RenderContext& context)
{
    vertex_count_ = vertex_idx_;
    index_count_ = index_idx_;
    context->BindMeshBuffer(vertex_buffer_.get(), index_buffer_.get());

    vertex_idx_ = 0;
    index_idx_ = 0;
}

int DrawBatcher::index_count() const
{
    return index_count_;
}
} // namespace blons