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

    // Dynamically resize our arrays as needed
    if (vertex_idx_ + vert_size > array_size_ ||
        index_idx_ + index_size > array_size_)
    {
        // TODO: Change this to *= 2? Maybe? Maybe not?
        array_size_ = std::max(vertex_idx_ + vert_size, index_idx_ + index_size);

        vertex_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
        index_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
        context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, array_size_, nullptr, array_size_);
        Start(context);

        /*auto new_vertices = std::unique_ptr<Vertex>((Vertex*)vptr);
        auto new_indices = std::unique_ptr<unsigned int>((unsigned int*)iptr);

        memcpy(new_vertices.get(), vertices_.get(), sizeof(Vertex) * vertex_idx_);
        memcpy(new_indices.get(), indices_.get(), sizeof(unsigned int) * index_idx_);

        vertices_ = std::move(new_vertices);
        indices_ = std::move(new_indices);*/
    }
    // memcpy is noticably faster in debug builds, not so much with compiler optimizations
    memcpy(vertices_+vertex_idx_, mesh_data.vertices.data(), sizeof(Vertex) * vert_size);
    // Caching these helps debug perf
    auto batch_indices_ptr = mesh_data.indices.data();
    // Can't memcpy here because indices need to be incremented as meshes are appended
    for (std::size_t j = 0; j < index_size; j++)
    {
        indices_[index_idx_ + j] = batch_indices_ptr[j] + vertex_idx_;
    }

    /*context->SetMeshData(vertex_buffer_.get(), index_buffer_.get(),
                         mesh_data.vertices.data(), vertex_idx_, vert_size,
                         indices_ptr+index_idx_, index_idx_, index_size);*/

    vertex_idx_ += vert_size;
    index_idx_ += index_size;
}

// TODO: This can be completely removed if we just store current map bindings as global state
// in RenderGL40
void DrawBatcher::Start(RenderContext& context)
{

    void* vptr = nullptr;
    void* iptr = nullptr;
    context->MapBufferResource(vertex_buffer_.get(), index_buffer_.get(), &vptr, &iptr);
    vertices_ = (Vertex*)vptr;
    indices_ = (unsigned int*)iptr;
}

void DrawBatcher::End(RenderContext& context)
{
    vertices_ = nullptr;
    indices_ = nullptr;
    context->MapBufferResource(nullptr, nullptr, nullptr, nullptr);
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