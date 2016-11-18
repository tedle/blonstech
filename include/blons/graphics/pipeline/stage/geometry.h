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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_GEOMETRY_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_GEOMETRY_H_

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
////////////////////////////////////////////////////////////////////////////////
/// \brief Renders out the g-buffer including albedo, normal, and depth
/// information to be consumed by later parts of the pipeline
////////////////////////////////////////////////////////////////////////////////
class Geometry
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output of the stage to retrieve
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        ALBEDO, ///< Albedo colour
        NORMAL, ///< World space normals
        DEPTH,  ///< 24-bit depth in the red channel
        DEBUG   ///< Various debug information
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new Geometry stage
    ///
    /// \param perspective Screen dimensions and perspective information
    ////////////////////////////////////////////////////////////////////////////////
    Geometry(Perspective perspective);
    ~Geometry() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders out the g-buffer
    ///
    /// \param scene Contains scene information for rendering
    /// \param view_matrix View matrix of the camera rendering the scene
    /// \param proj_matrix Perspective matrix for rendering the scene
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(const Scene& scene, Matrix view_matrix, Matrix proj_matrix);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer Geometry::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;

private:
    std::unique_ptr<Shader> geometry_shader_;
    std::unique_ptr<Framebuffer> geometry_buffer_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::Geometry
/// \ingroup pipeline
/// 
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_GEOMETRY_H_
