////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
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
// Local Includes
#include "resource.h"

namespace blons
{
Mesh::Mesh(const std::string& mesh_filename)
{
    filename_ = mesh_filename;
    Reload();
}

void Mesh::Reload()
{
    auto mesh = resource::LoadMesh(filename_);

    if (mesh.buffer == nullptr)
    {
        throw "Failed to initialize mesh";
    }

    buffer_ = std::move(mesh.buffer);
    vertex_count_ = mesh.vertex_count;
    index_count_ = mesh.index_count;
    data_ = std::move(mesh.data);
    texture_list_ = mesh.texture_list;
}

BufferResource* Mesh::buffer() const
{
    return buffer_.get();
}

unsigned int Mesh::vertex_count() const
{
    return vertex_count_;
}

unsigned int Mesh::index_count() const
{
    return index_count_;
}

const MeshData& Mesh::mesh() const
{
    return data_;
}

const std::vector<Mesh::TextureInfo>& Mesh::textures() const
{
    return texture_list_;
}
} // namespace blons
