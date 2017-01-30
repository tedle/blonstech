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

namespace blons
{
// Forward declarations
class Framebuffer;
class Shader;
class Texture3D;

namespace pipeline
{
namespace stage
{
// Forward declarations
class LightProbes;

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
        IRRADIANCE_VOLUME ///< 3D volume texture of indirect irradiance
    };

public:
    IrradianceVolume();
    ~IrradianceVolume() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Injects supplied light probe data and fills volume grid
    ///
    /// \param probes Handle to lightprobe data
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(const LightProbes& probes);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer IrradianceVolume::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;

private:
    std::unique_ptr<Texture3D> irradiance_volume_;
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
