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

namespace blons
{
// Forward declarations
class Framebuffer;
class Shader;

namespace pipeline
{
namespace stage
{
// Forward declarations
namespace debug { class ProbeView; }

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

private:
    struct Probe
    {
        const std::size_t id;
        Vector3 pos;
        float sh_sky_visibility[3 * 3];
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

private:
    // Since this is for debugging we can be alittle hacky with accessing dependencies
    friend debug::ProbeView;

    std::vector<Probe> probes_;
    std::unique_ptr<Framebuffer> environment_maps_;
    std::unique_ptr<Shader> environment_map_shader_;
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
