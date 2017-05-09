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

#include <blons/graphics/pipeline/stage/shadow.h>

// Public Includes
#include <blons/graphics/pipeline/stage/geometry.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
Shadow::Shadow(Perspective perspective)
{
    shadow_map_ortho_matrix_ = MatrixOrthographic(0, units::pixel_to_subpixel(kShadowMapResolution),
                                                  units::pixel_to_subpixel(kShadowMapResolution), 0,
                                                  perspective.screen_near, perspective.screen_far);

    ShaderAttributeList blur_inputs = { { POS, "input_pos" },
                                        { TEX, "input_uv" } };
    blur_shader_.reset(new Shader({ { VERTEX, "shaders/sprite.vert.glsl" }, { PIXEL, "shaders/shadow-blur.frag.glsl" } }, blur_inputs));

    ShaderAttributeList direct_light_inputs = { { POS, "input_pos" },
                                                { TEX, "input_uv" } };
    direct_light_shader_.reset(new Shader({ { VERTEX, "shaders/sprite.vert.glsl" }, { PIXEL, "shaders/direct-light.frag.glsl" } }, direct_light_inputs));

    ShaderAttributeList shadow_inputs = { { POS, "input_pos" } };
    shadow_shader_.reset(new Shader({ { VERTEX, "shaders/shadow.vert.glsl" }, { PIXEL, "shaders/shadow.frag.glsl" } }, shadow_inputs));

    if (blur_shader_ == nullptr ||
        direct_light_shader_ == nullptr ||
        shadow_shader_ == nullptr)
    {
        throw "Failed to initialize Shadow shaders";
    }

    blur_buffer_.reset(new Framebuffer(kShadowMapResolution, kShadowMapResolution, { { TextureType::R16G16_UNORM, TextureType::LINEAR, TextureType::CLAMP } }));
    direct_light_buffer_.reset(new Framebuffer(perspective.width, perspective.height, 1, false));
    shadow_buffer_.reset(new Framebuffer(kShadowMapResolution, kShadowMapResolution, { { TextureType::R16G16_UNORM, TextureType::LINEAR } }));
}

bool Shadow::Render(const Scene& scene, const Geometry& g_buffer, Matrix view_matrix, Matrix proj_matrix, Matrix light_vp_matrix, Matrix ortho_matrix)
{
    auto context = render::context();
    context->SetDepthTesting(true);
    context->SetBlendMode(BlendMode::OVERWRITE);
    // Can be removed when we support more lights
    assert(scene.lights.size() == 1);
    Light* sun = scene.lights[0];

    // TODO: Parallel split shadow maps
    //     Shouldn't be much harder than splitting clip distance in ndc_box of sun_->ViewFrustum
    //     along a linear blend of logarithmic and uniform splits
    // Bind the shadow depth framebuffer to render all models onto
    shadow_buffer_->Bind();

    // TODO: Separate into shadow.cpp and shadowmap.cpp for more modularity
    // TODO: 3D pass ->
    //      Render everything as a batch as this is untextured
    for (const auto& model : scene.models)
    {
        // Bind the vertex data
        model->Render();

        Matrix mvp_matrix = model->world_matrix() * light_vp_matrix;
        // Set the inputs
        if (!shadow_shader_->SetInput("mvp_matrix", mvp_matrix))
        {
            return false;
        }

        // Make the draw call
        if (!shadow_shader_->Render(model->index_count()))
        {
            return false;
        }
    }

    // Bit of an awkward hack to save VRAM:
    //     Bind blur buffer, use shadow depth as input and write a horizontal blur to blur buffer out
    //     Bind shadow buffer, use horizontal blur buffer as input and write a vertical blur to shadow buffer out
    // Result is an efficient O(2n) box blur (instead of O(n^2)!) needing only 2 textures (instead of 3)

    // Blur the shadow map to make soft shadows
    blur_buffer_->Bind();

    blur_buffer_->Render();

    // Horizontal blur
    if (!blur_shader_->SetInput("proj_matrix", shadow_map_ortho_matrix_) ||
        !blur_shader_->SetInput("blur_texture", shadow_buffer_->textures()[0]) ||
        !blur_shader_->SetInput("texture_resolution", kShadowMapResolution) ||
        !blur_shader_->SetInput("direction", 0))
    {
        return false;
    }
    if (!blur_shader_->Render(blur_buffer_->index_count()))
    {
        return false;
    }

    // Blur the shadow map to make soft shadows
    shadow_buffer_->Bind();

    shadow_buffer_->Render();

    // Veritcal blur
    if (!blur_shader_->SetInput("proj_matrix", shadow_map_ortho_matrix_) ||
        !blur_shader_->SetInput("blur_texture", blur_buffer_->textures()[0]) ||
        !blur_shader_->SetInput("texture_resolution", kShadowMapResolution) ||
        !blur_shader_->SetInput("direction", 1))
    {
        return false;
    }
    if (!blur_shader_->Render(shadow_buffer_->index_count()))
    {
        return false;
    }

    // Needed so sprites can render over themselves
    context->SetDepthTesting(false);

    // Bind the shadow depth framebuffer to render all models onto
    direct_light_buffer_->Bind();

    // Render the geometry as a sprite
    direct_light_buffer_->Render();

    // Used to turn pixel fragments into world coordinates
    Matrix inv_proj_view = MatrixInverse(view_matrix * proj_matrix);

    // Set the inputs
    if (!direct_light_shader_->SetInput("proj_matrix", ortho_matrix) ||
        !direct_light_shader_->SetInput("inv_vp_matrix", inv_proj_view) ||
        !direct_light_shader_->SetInput("light_vp_matrix", light_vp_matrix) ||
        !direct_light_shader_->SetInput("view_depth", g_buffer.output(Geometry::DEPTH), 0) ||
        !direct_light_shader_->SetInput("light_depth", output(LIGHT_DEPTH), 1))
    {
        return false;
    }

    // Finally do the render
    if (!direct_light_shader_->Render(direct_light_buffer_->index_count()))
    {
        return false;
    }
    return true;
}

const TextureResource* Shadow::output(Output buffer) const
{
    switch (buffer)
    {
    case LIGHT_DEPTH:
        return shadow_buffer_->textures()[0];
        break;
    case DIRECT_LIGHT:
        return direct_light_buffer_->textures()[0];
        break;
    default:
        return nullptr;
    }
}
} // namespace stage
} // namespace pipeline
} // namespace blons