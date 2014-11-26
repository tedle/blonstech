#include "graphics/render/drawbatcher.h"

namespace blons
{
DrawBatcher::DrawBatcher(RenderContext& context)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
    index_buffer_ = std::unique_ptr<BufferResource>(context->MakeBufferResource());
    context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, 0, nullptr, 0);
    batch_.resize(1000);
    batch_index_ = 0;
}

DrawBatcher::~DrawBatcher()
{
}

void DrawBatcher::Append(const MeshData& mesh_data)
{
    if (batch_index_ >= batch_.size())
    {
        batch_.resize(batch_.size() * 2);
    }
    batch_[batch_index_++] = mesh_data;
}

void DrawBatcher::Render(RenderContext& context)
{
    unsigned int vertex_offset = 0;
    unsigned int vertex_idx = 0, index_idx = 0;
    vertex_count_ = index_count_ = 0;
    for (std::size_t i = 0; i < batch_index_; i++)
    {
        vertex_count_ += batch_[i].vertices.size();
        index_count_ += batch_[i].indices.size();
    }
    vertices_ = std::unique_ptr<Vertex>(new Vertex[vertex_count_]);
    indices_ = std::unique_ptr<unsigned int>(new unsigned int[index_count_]);
    for (std::size_t i = 0; i < batch_index_; i++)
    {
        std::size_t vert_size = batch_[i].vertices.size();
        std::size_t index_size = batch_[i].indices.size();
        // memcpy is noticably faster in debug builds, not so much with compiler optimizations
        // cant do it for indices because they need to be incremented as the loop progresses
        memcpy(vertices_.get()+vertex_idx, batch_[i].vertices.data(), sizeof(Vertex) * vert_size);
        vertex_idx += vert_size;
        /*for (size_t j = 0; j < vert_size; j++)
        {
            vertices_.get()[vertex_idx++] = batch_[i].vertices[j];
        }*/
        // Caching these helps debug perf
        auto indices_ptr = indices_.get();
        auto batch_indices_ptr = batch_[i].indices.data();
        for (std::size_t j = 0; j < index_size; j++)
        {
            indices_ptr[index_idx++] = batch_indices_ptr[j] + vertex_offset;
        }
        vertex_offset += batch_[i].vertices.size();
    }

    context->SetMeshData(vertex_buffer_.get(), index_buffer_.get(),
                         vertices_.get(), vertex_count_,
                         indices_.get(), index_count_);;
    context->BindMeshBuffer(vertex_buffer_.get(), index_buffer_.get());

    // """"Reset"""" our silly array
    batch_index_ = 0;
}

int DrawBatcher::index_count() const
{
    return index_count_;
}
} // namespace blons