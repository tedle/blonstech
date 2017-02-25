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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_SHADOW_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_SHADOW_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/pipeline/stage/geometry.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Calculates cascading shadow maps and applys them to a scene
////////////////////////////////////////////////////////////////////////////////
class Shadow
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output of the stage to retrieve
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        LIGHT_DEPTH, ///< 16-bit depth in the red channel and 16-bit depth squared in the green channel
        DIRECT_LIGHT ///< R8G8B8 direct lighting. Only provides luminance
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new Shadow stage
    ///
    /// \param perspective Screen dimensions and perspective information
    ////////////////////////////////////////////////////////////////////////////////
    Shadow(Perspective perspective);
    ~Shadow() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders out the shadow map targets
    ///
    /// \param scene Contains scene information for rendering
    /// \param geometry Handle to the geometry buffer pass performed earlier in the
    /// frame
    /// \param view_matrix View matrix of the camera rendering the scene
    /// \param proj_matrix Perspective matrix for rendering the scene
    /// \param light_vp_matrix View projection matrix of the directional light
    /// providing shadow
    /// \param ortho_matrix Orthographic matrix bound to the screen dimensions
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(const Scene& scene, const Geometry& geometry, Matrix view_matrix, Matrix proj_matrix, Matrix light_vp_matrix, Matrix ortho_matrix);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer Shadow::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;

private:
    Matrix shadow_map_ortho_matrix_;

    std::unique_ptr<Shader> blur_shader_;
    std::unique_ptr<Shader> direct_light_shader_;
    std::unique_ptr<Shader> shadow_shader_;
    std::unique_ptr<Framebuffer> blur_buffer_;
    std::unique_ptr<Framebuffer> direct_light_buffer_;
    std::unique_ptr<Framebuffer> shadow_buffer_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::Shadow
/// \ingroup pipeline
/// 
/// ### Example:
/// See the pipeline module page for an example use on pipeline stages
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_SHADOW_H_