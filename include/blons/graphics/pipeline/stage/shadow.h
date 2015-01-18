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

#ifndef BLONSTECH_GRAPHICS_PIPELINE_STAGE_SHADOW_H_
#define BLONSTECH_GRAPHICS_PIPELINE_STAGE_SHADOW_H_

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
class Geometry;

class Shadow
{
public:
    enum Output
    {
        LIGHT_DEPTH,
        DIRECT_LIGHT
    };

public:
    Shadow(Perspective perspective, RenderContext& context);
    ~Shadow() {}

    bool Render(const Scene& scene, const Geometry& g_buffer, Matrix view_matrix, Matrix proj_matrix, Matrix light_vp_matrix, Matrix ortho_matrix, RenderContext& context);

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
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_PIPELINE_STAGE_SHADOW_H_