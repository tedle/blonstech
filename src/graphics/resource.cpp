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

#include "resource.h"

// Includes
#include <unordered_map>
// Public Includes
#include <blons/graphics/meshimporter.h>
// Local Includes
#include "internalresource.h"

namespace blons
{
namespace resource
{
namespace
{
struct MeshCache
{
    std::unique_ptr<MeshData> data;
    std::shared_ptr<BufferResource> vertex;
    std::shared_ptr<BufferResource> index;
    std::vector<Mesh::TextureInfo> texture_list;
};
struct TextureCache
{
    std::unique_ptr<PixelData> pixels;
    // TODO: Make this unique per context
    std::shared_ptr<TextureResource> texture;
};

std::unordered_map<std::string, MeshCache> g_mesh_cache;
std::unordered_map<std::string, TextureCache> g_texture_cache;
} // namespace

MeshBuffer LoadMesh(const std::string& filename, RenderContext& context)
{
    // Get texture data for file, or create fresh texture data if new
    auto& mesh = g_mesh_cache[filename];

    // Have we loaded the mesh data yet?
    if (mesh.data == nullptr)
    {
        if (internal::ValidEngineMesh(filename))
        {
            mesh.data.reset(new MeshData(internal::MakeEngineMesh(filename)));
        }
        else
        {
            MeshImporter blonsmesh(filename, true);
            MeshData mesh_data = blonsmesh.mesh_data();
            mesh.texture_list = blonsmesh.textures();
            mesh.data.reset(new MeshData(mesh_data));
        }

        // Graphics APIs dont support having more than 4 billion vertices...
        // I'm OK with that
        if (mesh.data->vertices.size() >= ULONG_MAX)
        {
            mesh.data.reset();
            mesh.texture_list.clear();
            return MeshBuffer();
        }
        if (mesh.data->indices.size() >= ULONG_MAX)
        {
            mesh.data.reset();
            mesh.texture_list.clear();
            return MeshBuffer();
        }
    }
    // Have we made a resource for this context?
    if (mesh.vertex == nullptr || mesh.index == nullptr)
    {
        std::shared_ptr<BufferResource> vertex(context->MakeBufferResource());
        std::shared_ptr<BufferResource> index(context->MakeBufferResource());
        if (!context->Register3DMesh(vertex.get(), index.get(),
                                     mesh.data->vertices.data(),
                                     static_cast<unsigned int>(mesh.data->vertices.size()),
                                     mesh.data->indices.data(),
                                     static_cast<unsigned int>(mesh.data->indices.size())))
        {
            return MeshBuffer();
        }
        mesh.vertex = std::move(vertex);
        mesh.index = std::move(index);
    }

    // Write the output info
    MeshBuffer buffer;
    buffer.vertex = mesh.vertex;
    buffer.index = mesh.index;
    buffer.vertex_count = static_cast<unsigned int>(mesh.data->vertices.size());
    buffer.index_count = static_cast<unsigned int>(mesh.data->indices.size());
    buffer.texture_list = mesh.texture_list;

    return buffer;
}

TextureBuffer LoadTexture(const std::string& filename, Texture::Type type, RenderContext& context)
{
    // Get texture data for file, or create fresh texture data if new
    auto& tex = g_texture_cache[filename];

    // Have we loaded the pixels yet?
    if (tex.pixels == nullptr)
    {
        if (internal::ValidEngineTexture(filename))
        {
            tex.pixels.reset(new PixelData(internal::MakeEngineTexture(filename)));
        }
        else
        {
            std::unique_ptr<PixelData> pixels(new PixelData);
            if (!context->LoadPixelData(filename, pixels.get()))
            {
                return TextureBuffer();
            }
            tex.pixels = std::move(pixels);
        }
    }

    // Have we made a resource for this context?
    if (tex.texture == nullptr)
    {
        std::shared_ptr<TextureResource> texture(context->MakeTextureResource());

        if (texture == nullptr)
        {
            return TextureBuffer();
        }

        if (type == Texture::SPRITE)
        {
            // No DDS compression or mipmaps + nearest neighbour filtering
            tex.pixels->compression = PixelData::RAW;
            tex.pixels->hint.filter = TextureHint::NEAREST;
        }
        else if (type == Texture::LIGHT)
        {
            // Get the most out of our lightmaps...
            tex.pixels->compression = PixelData::RAW;
        }

        if (!context->RegisterTexture(texture.get(), tex.pixels.get()))
        {
            return TextureBuffer();
        }

        // Save it to the cache
        tex.texture = std::move(texture);
    }

    // Write the output info
    TextureBuffer buffer;
    buffer.texture = tex.texture;
    buffer.info.width = tex.pixels->width;
    buffer.info.height = tex.pixels->height;
    buffer.info.type = type;

    return buffer;
}

void ClearBufferCache()
{
    for (auto& m : g_mesh_cache)
    {
        m.second.vertex.reset();
        m.second.index.reset();
    }
    for (auto& t : g_texture_cache)
    {
        t.second.texture.reset();
    }
}
} // namespace resource
} // namespace blons