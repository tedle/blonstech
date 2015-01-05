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
            unsigned int count = 10;
            float dist = (2 * kPi) / count;
            for (unsigned int i = 0; i < 1; i++)
            {
                for (unsigned int j = 0; j < count; j++)
                {
                    Vertex v;
                    v.pos = Vector3(sin(j * dist) * 10, cos(j * dist) * 10, tan(j * dist) * 10);

                    v.tex = Vector2(0.0, 0.0);
                    v.norm = Vector3(0.0, 0.0, 1.0);
                    v.tan = Vector3(1.0, 0.0, 0.0);
                    v.bitan = Vector3(0.0, 1.0, 0.0);
                }
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
            none.format = PixelData::AUTO;
            none.bits = PixelData::R8G8B8;
            none.width = 1;
            none.height = 1;
            none.pixels.reset(new unsigned char[none.width * none.height * 3]);
            none.pixels.get()[0] = 255;
            none.pixels.get()[1] = 0;
            none.pixels.get()[2] = 0;
            return none;
        }
    },
    {
        "blons:normal", []()
        {
            PixelData normal;
            normal.format = PixelData::AUTO;
            normal.bits = PixelData::R8G8B8;
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