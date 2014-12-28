////////////////////////////////////////////////////////////////////////////////
/// blonstech
/// Copyright(c) 2014 Dominic Bowden
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files(the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "resource.h"

// Includes
#include <unordered_map>

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

std::shared_ptr<TextureResource> LoadTexture(const std::string& filename, Texture::Type type, Texture::Info* info, RenderContext& context)
{
    // Get texture data for file, or create fresh texture data if new
    auto& tex = g_texture_cache[filename];

    // Have we loaded the pixels yet?
    if (tex.pixels == nullptr)
    {
        std::unique_ptr<PixelData> pixels(new PixelData);
        if (!context->LoadPixelData(filename, pixels.get()))
        {
            return nullptr;
        }

        // Save it to the cache
        tex.pixels = std::move(pixels);
    }

    // Have we made a resource for this context?
    if (tex.texture == nullptr)
    {
        std::shared_ptr<TextureResource> texture(context->MakeTextureResource());

        if (texture == nullptr)
        {
            return nullptr;
        }

        if (type == Texture::SPRITE)
        {
            // No DDS compression or mipmaps + nearest neighbour filtering
            tex.pixels->format = PixelData::RAW;
        }

        if (!context->RegisterTexture(texture.get(), tex.pixels.get()))
        {
            return nullptr;
        }

        // Save it to the cache
        tex.texture = texture;
    }

    // Write the output info
    info->width = tex.pixels->width;
    info->height = tex.pixels->height;
    info->type = type;

    return tex.texture;
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