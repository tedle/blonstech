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

#include <blons/graphics/pipeline/stage/irradiancevolume.h>

// Public Includes
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/texture3d.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
IrradianceVolume::IrradianceVolume()
{
    // Dummy volume built to wrap sponza in almost properly formed cubes
    PixelData3D dummy_volume;
    dummy_volume.width = 32;
    dummy_volume.height = 16;
    dummy_volume.depth = 16;
    dummy_volume.type = TextureType(TextureType::R8G8B8, TextureType::NEAREST);
    auto pixel_size = dummy_volume.bits_per_pixel() / 8;
    dummy_volume.pixels.reset(new unsigned char[dummy_volume.width * dummy_volume.height * dummy_volume.depth * pixel_size]);
    for (int z = 0; z < dummy_volume.depth; z++)
    {
        for (int y = 0; y < dummy_volume.height; y++)
        {
            for (int x = 0; x < dummy_volume.width; x++)
            {
                auto index = (z * dummy_volume.width * dummy_volume.height + y * dummy_volume.width + x) * pixel_size;
                // green channel only
                dummy_volume.pixels.get()[index+0] = 1;
                dummy_volume.pixels.get()[index+1] = 255;
                dummy_volume.pixels.get()[index+2] = 3;
            }
        }
    }
    irradiance_volume_.reset(new Texture3D(dummy_volume));

    // World matrix sized to wrap sponza scene
    static const units::world grid_width = 38.0f;
    static const units::world grid_height = 19.0f;
    static const units::world grid_depth = 18.0f;
    static const units::world x_offset = -grid_width / 2.0f;
    static const units::world y_offset = -2.0f;
    static const units::world z_offset = -grid_depth / 2.0f;
    world_matrix_ = MatrixScale(grid_width, grid_height, grid_depth) * MatrixTranslation(x_offset, y_offset, z_offset);
}

bool IrradianceVolume::Render(const LightProbes& probes)
{
    return true;
}

const TextureResource* IrradianceVolume::output(Output buffer) const
{
    return irradiance_volume_->texture();
}

Matrix IrradianceVolume::world_matrix() const
{
    return world_matrix_;
}
} // namespace stage
} // namespace pipeline
} // namespace blons
