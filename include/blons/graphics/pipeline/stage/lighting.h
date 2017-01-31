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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTING_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTING_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/geometry.h>
#include <blons/graphics/pipeline/stage/shadow.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Composites lighting information from previous passes and adds
/// specular
////////////////////////////////////////////////////////////////////////////////
class Lighting
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output of the stage to retrieve
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        LIGHT ///< Composited lighting information applied to the scene
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new Lighting stage
    ///
    /// \param perspective Screen dimensions and perspective information
    ////////////////////////////////////////////////////////////////////////////////
    Lighting(Perspective perspective);
    ~Lighting() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders out the composited lighting targets
    ///
    /// \param scene Contains scene information for rendering
    /// \param geometry Handle to the geometry buffer pass performed earlier in the
    /// frame
    /// \param shadow Handle to the shadow buffer pass performed earlier in the
    /// frame
    /// \param view_matrix View matrix of the camera rendering the scene
    /// \param proj_matrix Perspective matrix for rendering the scene
    /// \param ortho_matrix Orthographic matrix bound to the screen dimensions
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(const Scene& scene, const Geometry& geometry, const Shadow& shadow,
                Matrix view_matrix, Matrix proj_matrix, Matrix ortho_matrix);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer Lighting::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;

private:
    std::unique_ptr<Shader> light_shader_;
    std::unique_ptr<Framebuffer> light_buffer_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

/// \NEEDS_DOCUMENTATION
////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::Lighting
/// \ingroup pipeline
/// 
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_LIGHTING_H_
