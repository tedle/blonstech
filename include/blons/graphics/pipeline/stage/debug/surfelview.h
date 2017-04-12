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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_DEBUG_SURFELVIEW_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_DEBUG_SURFELVIEW_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>
#include <blons/graphics/render/shaderdata.h>
#include <blons/graphics/pipeline/stage/lightsector/lightsector.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace debug
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Generates debug info for LightSector::Surfel%s. Is already contained
/// within the general purpose DebugOutput class and doesn't require its own
/// instantiation
////////////////////////////////////////////////////////////////////////////////
class SurfelView
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new SurfelView stage
    ////////////////////////////////////////////////////////////////////////////////
    SurfelView();
    ~SurfelView() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders out the alpha blendable, overlaid debug information to the
    /// supplied Framebuffer
    ///
    /// \param target Framebuffer to render debug information onto
    /// \param depth Contains scene depth texture for depth testing 3D elements
    /// \param scene Contains scene information for rendering
    /// \param sector Handle to the light sector pass performed earlier in the
    /// frame
    /// \param view_matrix View matrix of the camera rendering the scene
    /// \param proj_matrix Perspective matrix for rendering the scene
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(Framebuffer* target, const TextureResource* depth, const Scene& scene, const LightSector& sector, Matrix view_matrix, Matrix proj_matrix);

private:
    std::unique_ptr<Shader> surfel_shader_;
    Mesh quad_mesh_;
};
} // namespace debug
} // namespace stage
} // namespace pipeline
} // namespace blons

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_DEBUG_SURFELVIEW_H_
