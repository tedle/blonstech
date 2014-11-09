#include "drawbatcher.h"

namespace blons
{
DrawBatcher::DrawBatcher(RenderContext& context)
{
    vertex_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    index_buffer_ = std::unique_ptr<BufferResource>(context->CreateBufferResource());
    context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, 0, nullptr, 0);
}

DrawBatcher::~DrawBatcher()
{
}

void DrawBatcher::Input(std::vector<MeshData>& mesh_data, RenderContext& context)
{
    unsigned int vertex_offset = 0;
    unsigned int vertex_idx = 0, index_idx = 0;
    vertex_count_ = index_count_ = 0;
    for (auto& m : mesh_data)
    {
        vertex_count_ += m.vertices.size();
        index_count_ += m.indices.size();
    }
    vertices_ = std::unique_ptr<Vertex>(new Vertex[vertex_count_]);
    indices_ = std::unique_ptr<unsigned int>(new unsigned int[index_count_]);
    for (auto& m : mesh_data)
    {
        // This is all C99 style stuff cus it runs like way faster for some reason
        // :(
        for (unsigned int i = 0; i < m.vertices.size(); i++)
        {
            vertices_.get()[vertex_idx++] = m.vertices[i];
        }
        for (unsigned int i = 0; i < m.indices.size(); i++)
        {
            indices_.get()[index_idx++] = m.indices[i] + vertex_offset;
        }
        vertex_offset += m.vertices.size();
    }

    context->SetMeshData(vertex_buffer_.get(), index_buffer_.get(),
                         vertices_.get(), vertex_count_,
                         indices_.get(), index_count_);;
    context->BindMeshBuffer(vertex_buffer_.get(), index_buffer_.get());
}

int DrawBatcher::index_count()
{
    return index_count_;
}

}