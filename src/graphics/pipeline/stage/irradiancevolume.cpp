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
    // Initialize 3D texture containing volume data
    PixelData3D volume;
    volume.width = kIrradianceVolumeWidth;
    volume.height = kIrradianceVolumeHeight;
    volume.depth = kIrradianceVolumeDepth;
    volume.type = TextureType(TextureType::R32G32B32A32, TextureType::LINEAR, TextureType::REPEAT);
    irradiance_volume_px_.reset(new Texture3D(volume));
    irradiance_volume_nx_.reset(new Texture3D(volume));
    irradiance_volume_py_.reset(new Texture3D(volume));
    irradiance_volume_ny_.reset(new Texture3D(volume));
    irradiance_volume_pz_.reset(new Texture3D(volume));
    irradiance_volume_nz_.reset(new Texture3D(volume));

    // World matrix sized to wrap sponza scene
    static const units::world grid_width = 38.0f;
    static const units::world grid_height = 19.0f;
    static const units::world grid_depth = 18.0f;
    static const units::world x_offset = -grid_width / 2.0f;
    static const units::world y_offset = -2.0f;
    static const units::world z_offset = -grid_depth / 2.0f;
    world_matrix_ = MatrixScale(grid_width, grid_height, grid_depth) * MatrixTranslation(x_offset, y_offset, z_offset);

    // Compute shader for updating irradiance volume each frame
    irradiance_volume_shader_.reset(new ComputeShader("shaders/irradiance-volume.comp.glsl"));
}

bool IrradianceVolume::Relight(const LightSector& sector)
{
    if (!irradiance_volume_shader_->SetInput("world_matrix", world_matrix_) ||
        !irradiance_volume_shader_->SetInput("probe_buffer", sector.probe_shader_data()) ||
        !irradiance_volume_shader_->SetOutput("irradiance_volume_px_out", irradiance_volume_px_->texture(), 0) ||
        !irradiance_volume_shader_->SetOutput("irradiance_volume_nx_out", irradiance_volume_nx_->texture(), 1) ||
        !irradiance_volume_shader_->SetOutput("irradiance_volume_py_out", irradiance_volume_py_->texture(), 2) ||
        !irradiance_volume_shader_->SetOutput("irradiance_volume_ny_out", irradiance_volume_ny_->texture(), 3) ||
        !irradiance_volume_shader_->SetOutput("irradiance_volume_pz_out", irradiance_volume_pz_->texture(), 4) ||
        !irradiance_volume_shader_->SetOutput("irradiance_volume_nz_out", irradiance_volume_nz_->texture(), 5))
    {
        return false;
    }
    irradiance_volume_shader_->Run(kIrradianceVolumeWidth, kIrradianceVolumeHeight, kIrradianceVolumeDepth);
    return true;
}

const TextureResource* IrradianceVolume::output(Output buffer) const
{
    switch (buffer)
    {
    case IRRADIANCE_VOLUME_PX:
        return irradiance_volume_px_->texture();
    case IRRADIANCE_VOLUME_NX:
        return irradiance_volume_nx_->texture();
    case IRRADIANCE_VOLUME_PY:
        return irradiance_volume_py_->texture();
    case IRRADIANCE_VOLUME_NY:
        return irradiance_volume_ny_->texture();
    case IRRADIANCE_VOLUME_PZ:
        return irradiance_volume_pz_->texture();
    case IRRADIANCE_VOLUME_NZ:
        return irradiance_volume_nz_->texture();
    default:
        throw "Requested non-existant output buffer from IrradianceVolume";
    }
}

Matrix IrradianceVolume::world_matrix() const
{
    return world_matrix_;
}
} // namespace stage
} // namespace pipeline
} // namespace blons
