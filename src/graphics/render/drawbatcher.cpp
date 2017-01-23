////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/graphics/render/drawbatcher.h>

// Includes
#include <algorithm>

namespace blons
{
DrawBatcher::DrawBatcher(Type type, DrawMode draw_mode)
{
    auto context = render::context();
    type_ = type;
    vertex_buffer_.reset(context->MakeBufferResource());
    index_buffer_.reset(context->MakeBufferResource());
    draw_mode_ = draw_mode;

    if (type_ == MESH_2D)
    {
        context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, 0, nullptr, 0, draw_mode_);
    }
    else if (type_ == MESH_3D)
    {
        context->Register3DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, 0, nullptr, 0, draw_mode_);
    }
    else
    {
        throw "Unsupported mesh type";
    }

    buffer_size_ = 0;
    vertex_count_ = 0;
    index_count_ = 0;
    vertex_idx_ = 0;
    index_idx_ = 0;
}

void DrawBatcher::Append(const MeshData& mesh_data, Matrix world_matrix)
{
    if (mesh_data.draw_mode != draw_mode_)
    {
        throw "DrawBatcher supplied with mismatched primitive types";
    }

    auto context = render::context();
    const unsigned int vert_size = static_cast<unsigned int>(mesh_data.vertices.size());
    const unsigned int index_size = static_cast<unsigned int>(mesh_data.indices.size());

    // Dynamically resize our buffers as needed
    if (vertex_idx_ + vert_size > buffer_size_ ||
        index_idx_ + index_size > buffer_size_)
    {
        // TODO: Change this to *= 2? Maybe? Maybe not?
        buffer_size_ = std::max(vertex_idx_ + vert_size, index_idx_ + index_size);

        // Make a backup copy of mesh data we've already pushed to render API
        Vertex* vptr = nullptr;
        unsigned int* iptr = nullptr;
        context->MapMeshData(vertex_buffer_.get(), index_buffer_.get(), &vptr, &iptr);
        auto old_vertices = std::unique_ptr<Vertex>(new Vertex[vertex_idx_]);
        auto old_indices = std::unique_ptr<unsigned int>(new unsigned int[index_idx_]);
        memcpy(old_vertices.get(), vptr, vertex_idx_ * sizeof(Vertex));
        memcpy(old_indices.get(), iptr, index_idx_ * sizeof(unsigned int));

        // Make the new, larger buffers
        vertex_buffer_.reset(context->MakeBufferResource());
        index_buffer_.reset(context->MakeBufferResource());
        if (type_ == MESH_2D)
        {
            context->Register2DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, buffer_size_, nullptr, buffer_size_, draw_mode_);
        }
        else if (type_ == MESH_3D)
        {
            context->Register3DMesh(vertex_buffer_.get(), index_buffer_.get(), nullptr, buffer_size_, nullptr, buffer_size_, draw_mode_);
        }

        // Move our backup copy of mesh data into the new buffer
        context->UpdateMeshData(vertex_buffer_.get(), index_buffer_.get(),
                                old_vertices.get(), 0, vertex_idx_,
                                old_indices.get(), 0, index_idx_);
    }
    Vertex* vertices;
    unsigned int* indices;
    context->MapMeshData(vertex_buffer_.get(), index_buffer_.get(), &vertices, &indices);

    // Append vertex data to the buffer
    memcpy(vertices + vertex_idx_, mesh_data.vertices.data(), sizeof(Vertex) * vert_size);
    if (world_matrix != MatrixIdentity())
    {
        for (std::size_t i = 0; i < vert_size; i++)
        {
            vertices[vertex_idx_ + i].pos = world_matrix * vertices[vertex_idx_ + i].pos;
        }
    }
    // Caching this helps debug perf
    auto mesh_indices_ptr = mesh_data.indices.data();
    // Can't memcpy here because indices need to be incremented as meshes are appended
    for (std::size_t j = 0; j < index_size; j++)
    {
        indices[index_idx_ + j] = mesh_indices_ptr[j] + vertex_idx_;
    }

    vertex_idx_ += vert_size;
    index_idx_ += index_size;
}

void DrawBatcher::Append(const MeshData& mesh_data)
{
    Append(mesh_data, MatrixIdentity());
}

void DrawBatcher::Render(bool clear_buffers)
{
    vertex_count_ = vertex_idx_;
    index_count_ = index_idx_;
    render::context()->BindMeshBuffer(vertex_buffer_.get(), index_buffer_.get());

    if (clear_buffers)
    {
        vertex_idx_ = 0;
        index_idx_ = 0;
    }
}

void DrawBatcher::Render()
{
    Render(true);
}

int DrawBatcher::index_count() const
{
    return index_count_;
}
} // namespace blons