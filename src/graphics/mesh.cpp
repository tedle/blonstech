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

#include <blons/graphics/mesh.h>

// Public Includes
#include <blons/graphics/meshimporter.h>

namespace blons
{
Mesh::Mesh(const MeshData& mesh_data, RenderContext& context)
{
    if (!Init(mesh_data, context))
    {
        throw "Failed to initialize mesh";
    }
}

bool Mesh::Init(const MeshData& mesh_data, RenderContext& context)
{
    vertex_buffer_.reset(context->MakeBufferResource());
    index_buffer_.reset(context->MakeBufferResource());
    mesh_data_.vertices = mesh_data.vertices;
    mesh_data_.indices = mesh_data.indices;

    // Graphics APIs dont support having more than 4 billion vertices...
    // I'm OK with that
    if (mesh_data_.vertices.size() >= ULONG_MAX)
    {
        return false;
    }

    if (mesh_data_.indices.size() >= ULONG_MAX)
    {
        return false;
    }

    if (!context->Register3DMesh(vertex_buffer_.get(), index_buffer_.get(),
                                 mesh_data_.vertices.data(), vertex_count(),
                                 mesh_data_.indices.data(), index_count()))
    {
        return false;
    }

    return true;
}

bool Mesh::Reload(RenderContext& context)
{
    // Make a copy of mesh data so we dont try to overwrite our own object, could be ugly?
    auto temp_data = mesh_data_;
    return Init(temp_data, context);
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
