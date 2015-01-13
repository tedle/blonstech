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

#include "internalresource.h"

// Includes
#include <functional>
#include <unordered_map>

namespace blons
{
namespace resource
{
namespace internal
{
namespace
{
std::unordered_map<std::string, std::function<MeshData()>> g_mesh_generators =
{
    {
        "blons:sphere", []()
        {
            MeshData sphere;

            const unsigned int count = 20;
            const unsigned int semi_count = count / 2 + 1;
            const unsigned int vert_count = count * semi_count;
            const float scale = 1.0f;
            const float dist = (2 * kPi) / count;

            static_assert(count % 2 == 0, "Sphere vertex ring size must be an even number");

            // count + 1 to generate a duplicate origin ring
            // this allows a starting ring with UV.x = 0 and
            // an ending ring with UV.x = 1
            for (unsigned int i = 0; i < count + 1; i++)
            {
                for (unsigned int j = 0; j < semi_count; j++)
                {
                    // Offset by half pi because semispheres are parallel
                    // to the Y-axis
                    float d1 = j * dist + kPi / 2;
                    float d2 = i * dist + kPi / 2;
                    Vertex v;
                    v.pos.x = cos(d1) * cos(d2);
                    v.pos.y = sin(d1);
                    v.pos.z = cos(d1) * sin(d2);
                    v.pos *= scale;

                    v.tex = Vector2(static_cast<float>(i) / static_cast<float>(count),
                                    static_cast<float>(j) / static_cast<float>(semi_count));
                    v.norm = VectorNormalize(v.pos);
                    v.tan = Vector3(1.0, 0.0, 0.0);
                    v.bitan = Vector3(0.0, 1.0, 0.0);
                    sphere.vertices.push_back(v);
                }
            }

            for (unsigned int i = 0; i < vert_count; i++)
            {
                // Don't need modulo for these because of the bonus ring we generated
                sphere.indices.push_back(i);
                sphere.indices.push_back(i + semi_count);
                sphere.indices.push_back(i + 1);

                sphere.indices.push_back(i + 1);
                sphere.indices.push_back(i + semi_count);
                sphere.indices.push_back(i + semi_count + 1);
            }
            return sphere;
        }
    }
};
std::unordered_map<std::string, std::function<PixelData()>> g_texture_generators =
{
    {
        "blons:none", []()
        {
            PixelData none;
            none.compression = PixelData::RAW;
            none.hint.format = TextureHint::R8G8B8;
            none.hint.filter = TextureHint::NEAREST;
            none.width = 16;
            none.height = 16;
            none.pixels.reset(new unsigned char[none.width * none.height * 3]);
            for (int x = 0; x < none.width; x++)
            {
                for (int y = 0; y < none.height; y++)
                {
                    auto offset = y * none.width + x;
                    none.pixels.get()[offset * 3 + 0] = 100 * ((y + x) % 2);
                    none.pixels.get()[offset * 3 + 1] = 200 * ((y + x) % 2);
                    none.pixels.get()[offset * 3 + 2] = 100 * ((y + x) % 2);
                }
            }
            return none;
        }
    },
    {
        "blons:normal", []()
        {
            PixelData normal;
            normal.compression = PixelData::RAW;
            normal.hint.format = TextureHint::R8G8B8;
            normal.hint.filter = TextureHint::LINEAR;
            normal.width = 1;
            normal.height = 1;
            normal.pixels.reset(new unsigned char[normal.width * normal.height * 3]);
            normal.pixels.get()[0] = 128;
            normal.pixels.get()[1] = 128;
            normal.pixels.get()[2] = 255;
            return normal;
        }
    }
};
} // namespace

MeshData MakeEngineMesh(const std::string& name)
{
    return g_mesh_generators.at(name)();
}

bool ValidEngineMesh(const std::string& name)
{
    return (g_mesh_generators.find(name) != g_mesh_generators.end());
}

PixelData MakeEngineTexture(const std::string& name)
{
    return g_texture_generators.at(name)();
}

bool ValidEngineTexture(const std::string& name)
{
    return (g_texture_generators.find(name) != g_texture_generators.end());
}
} // namespace internal
} // namespace resource
} // namespace blons