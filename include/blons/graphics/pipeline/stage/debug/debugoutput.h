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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_DEBUG_DEBUGOUTPUT_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_DEBUG_DEBUGOUTPUT_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/lightsector/lightsector.h>
#include <blons/graphics/pipeline/stage/irradiancevolume.h>
#include <blons/graphics/pipeline/stage/debug/surfelview.h>
#include <blons/graphics/pipeline/stage/debug/probeview.h>
#include <blons/graphics/pipeline/stage/debug/irradianceview.h>
#include <blons/graphics/framebuffer.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace debug
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Composites all active debug information into an alpha blendable
/// Framebuffer that can be overlaid on top of the scene
////////////////////////////////////////////////////////////////////////////////
class DebugOutput
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output of the stage to retrieve
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        DEBUG ///< Alpha blendable debug information
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new DebugOutput stage
    ///
    /// \param perspective Screen dimensions and perspective information
    ////////////////////////////////////////////////////////////////////////////////
    DebugOutput(Perspective perspective);
    ~DebugOutput() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders out the alpha blendable, overlaid debug information
    ///
    /// \param depth Contains scene depth texture for depth testing 3D elements
    /// \param scene Contains scene information for rendering
    /// \param sector Handle to the light sector pass performed earlier in the
    /// frame
    /// \param irradiance Handle to the irradiance volume pass performed earlier in
    /// the frame
    /// \param view_matrix View matrix of the camera rendering the scene
    /// \param proj_matrix Perspective matrix for rendering the scene
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(const TextureResource* depth, const Scene& scene, const LightSector& sector, const IrradianceVolume& irradiance, Matrix view_matrix, Matrix proj_matrix);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer DebugOutput::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;

private:
    std::unique_ptr<Framebuffer> debug_output_buffer_;
    std::unique_ptr<SurfelView> surfelview_;
    std::unique_ptr<ProbeView> probeview_;
    std::unique_ptr<IrradianceView> irradianceview_;
};
} // namespace debug
} // namespace stage
} // namespace pipeline
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::debug::DebugOutput
/// \ingroup pipeline
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_DEBUG_DEBUGOUTPUT_H_
