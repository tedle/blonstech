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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_IRRADIANCEVOLUME_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_IRRADIANCEVOLUME_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/lightprobes.h>
#include <blons/graphics/texture3d.h>
#include <blons/graphics/render/computeshader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Turns blons::pipeline::stage::LightProbe data into a volumetric grid
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
    /// \param probes Handle to lightprobe data
    ////////////////////////////////////////////////////////////////////////////////
    bool Relight(const LightProbes& probes);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer IrradianceVolume::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;
    Matrix world_matrix() const;

private:
    std::unique_ptr<Texture3D> irradiance_volume_px_; // Stores ambient cube coefficient for +X
    std::unique_ptr<Texture3D> irradiance_volume_nx_; // Stores ambient cube coefficient for -X
    std::unique_ptr<Texture3D> irradiance_volume_py_; // Stores ambient cube coefficient for +Y
    std::unique_ptr<Texture3D> irradiance_volume_ny_; // Stores ambient cube coefficient for -Y
    std::unique_ptr<Texture3D> irradiance_volume_pz_; // Stores ambient cube coefficient for +Z
    std::unique_ptr<Texture3D> irradiance_volume_nz_; // Stores ambient cube coefficient for -Z
    std::unique_ptr<ComputeShader> irradiance_volume_shader_;
    Matrix world_matrix_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

/// \NEEDS_DOCUMENTATION
////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::IrradianceVolume
/// \ingroup pipeline
///
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_IRRADIANCEVOLUME_H_
