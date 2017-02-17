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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTPROBES_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTPROBES_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>
#include <blons/graphics/render/shaderdata.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Manages light probes used to calculate indirect illumination at
/// runtime with the use of precomputed radiance transfer
////////////////////////////////////////////////////////////////////////////////
class LightProbes
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output of the stage to retrieve
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        ENV_MAPS ///< Cached albedo environment maps
    };

    struct Probe
    {
        const int id;
        Vector3 pos;
        AmbientCube irradiance;
        SHCoeffs3 sh_sky_visibility;
    };

public:
    LightProbes();
    ~LightProbes() {}

    void BakeRadianceTransfer(const Scene& scene);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer Lighting::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;

    const std::vector<Probe>& probes() const;
    const ShaderDataResource* probe_shader_data() const;

private:
    std::vector<Probe> probes_;
    std::unique_ptr<Framebuffer> environment_maps_;
    std::unique_ptr<Shader> environment_map_shader_;
    std::unique_ptr<ShaderData<Probe>> probe_shader_data_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

/// \NEEDS_DOCUMENTATION
////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::LightProbes
/// \ingroup pipeline
///
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTPROBES_H_
