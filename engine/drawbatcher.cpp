#include "drawbatcher.h"

namespace blons
{
DrawBatcher::DrawBatcher(RenderContext& context)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    index_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, 0, nullptr, 0);
    batch_.resize(1000);
    batch_index_ = 0;
}

DrawBatcher::~DrawBatcher()
{
}

void DrawBatcher::Append(MeshData* mesh_data)
{
    if (batch_index_ >= batch_.size())
    {
        batch_.resize(batch_.size() * 2);
    }
    batch_[batch_index_++] = *mesh_data;
}

void DrawBatcher::Render(RenderContext& context)
{
    unsigned int vertex_offset = 0;
    unsigned int vertex_idx = 0, index_idx = 0;
    vertex_count_ = index_count_ = 0;
    for (size_t i = 0; i < batch_index_; i++)
    {
        vertex_count_ += batch_[i].vertices.size();
        index_count_ += batch_[i].indices.size();
    }
    vertices_ = std::unique_ptr<Vertex>(new Vertex[vertex_count_]);
    indices_ = std::unique_ptr<unsigned int>(new unsigned int[index_count_]);
    for (size_t i = 0; i < batch_index_; i++)
    {
        for (size_t j = 0; j < batch_[i].vertices.size(); j++)
        {
            vertices_.get()[vertex_idx++] = batch_[i].vertices[j];
        }
        for (size_t j = 0; j < batch_[i].indices.size(); j++)
        {
            indices_.get()[index_idx++] = batch_[i].indices[j] + vertex_offset;
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

int DrawBatcher::index_count()
{
    return index_count_;
}

}