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
struct TextureCache
{
    std::unique_ptr<PixelData> pixels;
    // TODO: Make this unique per context
    std::shared_ptr<TextureResource> texture;
};
std::unordered_map<std::string, TextureCache> g_texture_cache;
} // namespace

MeshBuffer LoadMesh(const std::string& filename, RenderContext& context)
{
    MeshBuffer buffer;
    MeshImporter blonsmesh(filename, true);
    MeshData mesh_data = blonsmesh.mesh_data();
    buffer.texture_list = blonsmesh.textures();

    // Graphics APIs dont support having more than 4 billion vertices...
    // I'm OK with that
    if (mesh_data.vertices.size() >= ULONG_MAX)
    {
        return MeshBuffer();
    }
    buffer.vertex_count = static_cast<unsigned int>(mesh_data.vertices.size());

    if (mesh_data.indices.size() >= ULONG_MAX)
    {
        return MeshBuffer();
    }
    buffer.index_count = static_cast<unsigned int>(mesh_data.indices.size());

    buffer.vertex.reset(context->MakeBufferResource());
    buffer.index.reset(context->MakeBufferResource());

    if (!context->Register3DMesh(buffer.vertex.get(), buffer.index.get(),
                                 mesh_data.vertices.data(), buffer.vertex_count,
                                 mesh_data.indices.data(), buffer.index_count))
    {
        return MeshBuffer();
    }

    return buffer;
}

TextureBuffer LoadTexture(const std::string& filename, Texture::Type type, RenderContext& context)
{
    TextureBuffer buffer;

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
            tex.pixels->format = PixelData::RAW;
        }

        if (!context->RegisterTexture(texture.get(), tex.pixels.get()))
        {
            return TextureBuffer();
        }

        // Save it to the cache
        tex.texture = texture;
    }

    // Write the output info
    buffer.texture = tex.texture;
    buffer.info.width = tex.pixels->width;
    buffer.info.height = tex.pixels->height;
    buffer.info.type = type;

    return buffer;
}

void ClearBufferCache()
{
    for (auto& t : g_texture_cache)
    {
        t.second.texture.reset();
    }
}
} // namespace resource
} // namespace blons