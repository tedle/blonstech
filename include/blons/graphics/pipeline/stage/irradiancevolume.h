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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_IRRADIANCEVOLUME_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_IRRADIANCEVOLUME_H_

// Includes
#include <array>
// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/lightsector/lightsector.h>
#include <blons/graphics/texture3d.h>
#include <blons/graphics/render/computeshader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Turns blons::pipeline::stage::LightSector data into a volumetric grid
/// of directional light for shading
////////////////////////////////////////////////////////////////////////////////
class IrradianceVolume
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output of the stage to retrieve
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        IRRADIANCE_VOLUME_PX = AxisAlignedNormal::POSITIVE_X, ///< 3D volume texture of ambient cube coefficient for +X
        IRRADIANCE_VOLUME_NX = AxisAlignedNormal::NEGATIVE_X, ///< 3D volume texture of ambient cube coefficient for -X
        IRRADIANCE_VOLUME_PY = AxisAlignedNormal::POSITIVE_Y, ///< 3D volume texture of ambient cube coefficient for +Y
        IRRADIANCE_VOLUME_NY = AxisAlignedNormal::NEGATIVE_Y, ///< 3D volume texture of ambient cube coefficient for -Y
        IRRADIANCE_VOLUME_PZ = AxisAlignedNormal::POSITIVE_Z, ///< 3D volume texture of ambient cube coefficient for +Z
        IRRADIANCE_VOLUME_NZ = AxisAlignedNormal::NEGATIVE_Z  ///< 3D volume texture of ambient cube coefficient for -Z
    };

public:
    IrradianceVolume();
    ~IrradianceVolume() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Injects supplied light probe data and fills volume grid
    ///
    /// \param sector Handle to sector data
    ////////////////////////////////////////////////////////////////////////////////
    bool Relight(const LightSector& sector);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer IrradianceVolume::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a world matrix that scales and transforms a cube with
    /// vertices of [0,1] to a rectangular prism containing this IrradianceVolume.
    /// Used for transforming UVW space positions to world space positions
    ///
    /// \return Irradiance volume world matrix
    ////////////////////////////////////////////////////////////////////////////////
    Matrix world_matrix() const;

private:
    std::array<std::unique_ptr<Texture3D>, 6> irradiance_volume_; // Stores all 6 ambient cube coefficients
    std::unique_ptr<ComputeShader> irradiance_volume_shader_;
    Matrix world_matrix_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::IrradianceVolume
/// \ingroup pipeline
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_IRRADIANCEVOLUME_H_
