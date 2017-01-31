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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_COMPOSITE_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_COMPOSITE_H_

// Public Includes
#include <blons/graphics/pipeline/scene.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>

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
class Composite
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to specify which output of the stage to retrieve
    ////////////////////////////////////////////////////////////////////////////////
    enum Output
    {
        FINAL ///< Final composite of all shading passes
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new Composite stage
    ///
    /// \param perspective Screen dimensions and perspective information
    ////////////////////////////////////////////////////////////////////////////////
    Composite(Perspective perspective);
    ~Composite() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders out the composited shading targets
    ///
    /// \param scene Contains texture of rendered and shaded scene
    /// \param debug_output Any debug information to be overlayed onto scene
    /// \param ortho_matrix Orthographic matrix bound to the screen dimensions
    ////////////////////////////////////////////////////////////////////////////////
    bool Render(const TextureResource* scene, const TextureResource* debug_output, Matrix ortho_matrix);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the rendering output from the pipeline stage
    ///
    /// \param buffer Composite::Output target to retrieve
    /// \return Handle to the output target texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* output(Output buffer) const;

private:
    std::unique_ptr<Shader> composite_shader_;
    std::unique_ptr<Framebuffer> composite_buffer_;
};
} // namespace stage
} // namespace pipeline
} // namespace blons

/// \NEEDS_DOCUMENTATION
////////////////////////////////////////////////////////////////////////////////
/// \class blons::pipeline::stage::Composite
/// \ingroup pipeline
/// 
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_COMPOSITE_H_
