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
std::unordered_map<std::string, std::function<MeshData(const std::vector<std::string>& args)>> g_mesh_generators =
{
    {
        "blons:cube", [](const std::vector<std::string>& args)
        {
            if (args.size() != 0)
            {
                throw "Incorrect arguments supplied for blons:quad mesh (expected 0)";
            }

            MeshData quad = MakeEngineMesh("blons:quad");
            for (auto& v : quad.vertices)
            {
                v.pos *= MatrixTranslation(0.0f, 0.0f, 1.0f);
            }

            MeshData cube;
            cube.draw_mode = DrawMode::TRIANGLES;

            for (int face = 0; face < 6; face++)
            {
                units::world pitch = 0.0f;
                units::world yaw = 0.0f;
                switch (face)
                {
                case 0:
                    break;
                case 1:
                    yaw = kPi / 2.0f;
                    break;
                case 2:
                    yaw = kPi;
                    break;
                case 3:
                    yaw = -kPi / 2.0f;
                    break;
                case 4:
                    pitch = kPi / 2.0f;
                    break;
                case 5:
                    pitch = -kPi / 2.0f;
                    break;
                default:
                    throw "Bad face index during cube generation";
                }
                auto view = MatrixView(Vector3(0), Vector3(pitch, yaw, 0.0f));
                for (const auto& vert : quad.vertices)
                {
                    Vertex v;
                    v.pos = vert.pos * view;
                    v.tex = vert.tex;
                    v.norm = vert.norm * view;
                    v.tan = vert.tan * view;
                    v.bitan = vert.bitan * view;
                    cube.vertices.push_back(v);
                }
                for (int i = 0; i < 6; i++)
                {
                    cube.indices.push_back(quad.indices[i] + (face * 4));
                }
            }

            return cube;
        }
    },
    {
        "blons:line-grid", [](const std::vector<std::string>& args)
        {
            if (args.size() != 3)
            {
                throw "Incorrect arguments supplied for blons:line-grid mesh (expected 3)";
            }

            MeshData grid;
            grid.draw_mode = DrawMode::LINES;

            const std::size_t xsize = static_cast<std::size_t>(atoi(args[0].c_str()));
            const std::size_t ysize = static_cast<std::size_t>(atoi(args[1].c_str()));
            const std::size_t zsize = static_cast<std::size_t>(atoi(args[2].c_str()));
            unsigned int index = 0;
            for (int x = 0; x <= xsize; x++)
            {
                for (int y = 0; y <= ysize; y++)
                {
                    for (int z = 0; z <= zsize; z++)
                    {
                        Vertex v1, v2;
                        units::world wx, wy, wz;
                        unsigned int v1_index = index;
                        wx = static_cast<units::world>(x) / static_cast<units::world>(xsize);
                        wy = static_cast<units::world>(y) / static_cast<units::world>(ysize);
                        wz = static_cast<units::world>(z) / static_cast<units::world>(zsize);
                        v1.pos = Vector3(wx, wy, wz);
                        grid.vertices.push_back(v1);
                        index++;

                        if (x < xsize)
                        {
                            wx += 1.0f / static_cast<units::world>(xsize);
                            v2.pos = Vector3(wx, v1.pos.y, v1.pos.z);
                            grid.vertices.push_back(v2);
                            grid.indices.push_back(v1_index);
                            grid.indices.push_back(index);
                            index++;
                        }
                        if (y < ysize)
                        {
                            wy += 1.0f / static_cast<units::world>(ysize);
                            v2.pos = Vector3(v1.pos.x, wy, v1.pos.z);
                            grid.vertices.push_back(v2);
                            grid.indices.push_back(v1_index);
                            grid.indices.push_back(index);
                            index++;
                        }
                        if (z < zsize)
                        {
                            wz += 1.0f / static_cast<units::world>(zsize);
                            v2.pos = Vector3(v1.pos.x, v1.pos.y, wz);
                            grid.vertices.push_back(v2);
                            grid.indices.push_back(v1_index);
                            grid.indices.push_back(index);
                            index++;
                        }
                    }
                }
            }
            return grid;
        }
    },
    {
        "blons:quad", [](const std::vector<std::string>& args)
        {
            if (args.size() != 0)
            {
                throw "Incorrect arguments supplied for blons:quad mesh (expected 0)";
            }

            MeshData quad;
            quad.draw_mode = DrawMode::TRIANGLES;

            Vertex v;
            v.pos = Vector3(-1.0, -1.0, 0.0);
            v.tex = Vector2(0.0, 0.0);
            v.norm = Vector3(0.0, 0.0, 1.0);
            v.tan = Vector3(1.0, 0.0, 0.0);
            v.bitan = Vector3(0.0, 1.0, 0.0);
            quad.vertices.push_back(v);

            v.pos = Vector3(1.0, -1.0, 0.0);
            v.tex = Vector2(1.0, 0.0);
            quad.vertices.push_back(v);

            v.pos = Vector3(-1.0, 1.0, 0.0);
            v.tex = Vector2(0.0, 1.0);
            quad.vertices.push_back(v);

            v.pos = Vector3(1.0, 1.0, 0.0);
            v.tex = Vector2(1.0, 1.0);
            quad.vertices.push_back(v);

            quad.indices = { 0, 1, 2, 2, 1, 3 };

            return quad;
        }
    },
    {
        "blons:sphere", [](const std::vector<std::string>& args)
        {
            if (args.size() != 0)
            {
                throw "Incorrect arguments supplied for blons:sphere mesh (expected 0)";
            }

            MeshData sphere;
            sphere.draw_mode = DrawMode::TRIANGLES;

            const unsigned int count = 20;
            const unsigned int semi_count = count / 2 + 1;
            const unsigned int vert_count = count * semi_count;
            const float radius = 1.0f;
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
                    v.pos *= radius;

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
            // We do need a modulo on the very last index
            sphere.indices.back() = 0;

            return sphere;
        }
    }
};
std::unordered_map<std::string, std::function<PixelData(const std::vector<std::string>& args)>> g_texture_generators =
{
    {
        "blons:none", [](const std::vector<std::string>& args)
        {
            if (args.size() != 0)
            {
                throw "Incorrect arguments supplied for blons:none texture (expected 0)";
            }

            PixelData none;
            none.type.format = TextureType::R8G8B8;
            none.type.compression = TextureType::RAW;
            none.type.filter = TextureType::NEAREST;
            none.type.wrap = TextureType::REPEAT;
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
        "blons:normal", [](const std::vector<std::string>& args)
        {
            if (args.size() != 0)
            {
                throw "Incorrect arguments supplied for blons:normal texture (expected 0)";
            }

            PixelData normal;
            normal.type.format = TextureType::R8G8B8;
            normal.type.compression = TextureType::RAW;
            normal.type.filter = TextureType::LINEAR;
            normal.type.wrap = TextureType::REPEAT;
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

struct EngineAsset
{
    std::string func;
    std::vector<std::string> args;
};

// Form EngineAsset based on func~arg,arg,arg syntax
EngineAsset ParseEngineAsset(const std::string& name)
{
    EngineAsset asset;
    auto split = name.find_first_of('~');
    asset.func = name.substr(0, split);
    while (split != std::string::npos)
    {
        auto start = split + 1;
        split = name.find_first_of(',', start);
        auto segment_length = split != std::string::npos ? split - start : split;
        asset.args.push_back(name.substr(start, segment_length));
    }
    return asset;
}
} // namespace

MeshData MakeEngineMesh(const std::string& name)
{
    auto asset = ParseEngineAsset(name);
    return g_mesh_generators.at(asset.func)(asset.args);
}

bool ValidEngineMesh(const std::string& name)
{
    auto asset = ParseEngineAsset(name);
    return (g_mesh_generators.find(asset.func) != g_mesh_generators.end());
}

PixelData MakeEngineTexture(const std::string& name)
{
    auto asset = ParseEngineAsset(name);
    return g_texture_generators.at(asset.func)(asset.args);
}

bool ValidEngineTexture(const std::string& name)
{
    auto asset = ParseEngineAsset(name);
    return (g_texture_generators.find(asset.func) != g_texture_generators.end());
}
} // namespace internal
} // namespace resource
} // namespace blons