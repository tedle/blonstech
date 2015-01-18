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

#include <blons/graphics/pipeline/deferred.h>

// Public Includes
#include <blons/graphics/pipeline/stage/geometry.h>
#include <blons/graphics/pipeline/stage/shadow.h>
#include <blons/graphics/pipeline/stage/lightprobe.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>
// TODO: Remove this include when we get smarter debug render targets
#include <blons/graphics/sprite.h>

namespace blons
{
namespace pipeline
{
Deferred::Deferred(Client::Info screen, float fov, float screen_near, float screen_far, RenderContext& context)
{
    Reload(screen, fov, screen_near, screen_far, context);
}

Deferred::~Deferred() {}

bool Deferred::Init(RenderContext& context)
{
    // Projection matrix (3D space->2D screen)
    float screen_aspect = static_cast<float>(perspective_.width) / static_cast<float>(perspective_.height);
    proj_matrix_ = MatrixPerspective(perspective_.fov, screen_aspect, perspective_.screen_near, perspective_.screen_far);

    // Ortho projection matrix (for 2d stuff, shadow maps, etc)
    ortho_matrix_ = MatrixOrthographic(0, units::pixel_to_subpixel(perspective_.width), units::pixel_to_subpixel(perspective_.height), 0,
                                       perspective_.screen_near, perspective_.screen_far);

    // Pipeline setup
    geometry_.reset(new stage::Geometry(perspective_, context));
    shadow_.reset(new stage::Shadow(perspective_, context));
    lightprobe_.reset(new stage::Lightprobe(perspective_, context));

    // Shaders
    ShaderAttributeList light_inputs;
    light_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    light_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    light_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/light.frag.glsl", light_inputs, context));

    ShaderAttributeList composite_inputs;
    composite_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    composite_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    composite_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/sprite.frag.glsl", composite_inputs, context));

    if (light_shader_ == nullptr ||
        composite_shader_ == nullptr)
    {
        return false;
    }

    // Framebuffers
    light_buffer_.reset(new Framebuffer(perspective_.width, perspective_.height, 1, false, context));

    return true;
}

void Deferred::Reload(Client::Info screen, float fov, float screen_near, float screen_far, RenderContext& context)
{
    perspective_.width = screen.width;
    perspective_.height = screen.height;
    perspective_.screen_near = screen_near;
    perspective_.screen_far = screen_far;
    perspective_.fov = fov;

    Init(context);
}

bool Deferred::Render(const Scene& scene, RenderContext& context)
{
    // TODO: Support more scene lights
    assert(scene.lights.size() == 1);
    Light* sun = scene.lights[0];

    // Calculates view_matrix from scratch, so we cache it
    Matrix view_matrix = scene.view.view_matrix();
    Matrix light_vp_matrix = sun->ViewFrustum(view_matrix * proj_matrix_, perspective_.screen_far);

    // Render all of the geometry and accompanying info (normal, depth, etc)
    if (!geometry_->Render(scene, view_matrix, proj_matrix_, context))
    {
        return false;
    }

    // Render all of the geometry and get their depth from the light's point of view
    // Then render a shadow map from the depth information
    if (!shadow_->Render(scene, *geometry_, view_matrix, proj_matrix_, light_vp_matrix, ortho_matrix_, context))
    {
        return false;
    }

    // Builds a direct light map and then bounce lighting
    if (!lightprobe_->Render(scene, *geometry_, *shadow_, perspective_,
                             view_matrix, proj_matrix_, light_vp_matrix, context))
    {
        return false;
    }

    if (!RenderLighting(scene, view_matrix, context))
    {
        return false;
    }

    // Bind the back buffer
    context->BindFramebuffer(nullptr);

    // Render the final composite of the geometry and lighting passes
    if (!RenderComposite(scene, context))
    {
        return false;
    }

    return true;
}

bool Deferred::BuildLighting(const Scene& scene, RenderContext& context)
{
    return lightprobe_->BuildLighting(scene, context);
}

bool Deferred::RenderLighting(const Scene& scene, Matrix view_matrix, RenderContext& context)
{
    // Can be removed when we support more lights
    assert(scene.lights.size() == 1);
    Light* sun = scene.lights[0];

    // Used to turn pixel fragments into world coordinates
    Matrix inv_proj_view = MatrixInverse(view_matrix * proj_matrix_);

    // Bind the buffer to do all lighting calculations on
    light_buffer_->Bind(context);

    light_buffer_->Render(context);

    // Set the inputs
    if (!light_shader_->SetInput("proj_matrix", ortho_matrix_, context) ||
        !light_shader_->SetInput("inv_vp_matrix", inv_proj_view, context) ||
        !light_shader_->SetInput("albedo", geometry_->output(stage::Geometry::ALBEDO), 0, context) ||
        !light_shader_->SetInput("normal", geometry_->output(stage::Geometry::NORMAL), 1, context) ||
        !light_shader_->SetInput("depth", geometry_->output(stage::Geometry::DEPTH), 2, context) ||
        !light_shader_->SetInput("direct_light", shadow_->output(stage::Shadow::DIRECT_LIGHT), 3, context) ||
        !light_shader_->SetInput("indirect_light", lightprobe_->output(stage::Lightprobe::INDIRECT_LIGHT), 4, context) ||
        !light_shader_->SetInput("sun.dir", sun->direction(), context) ||
        !light_shader_->SetInput("sun.colour", sun->colour(), context) ||
        !light_shader_->SetInput("sky_colour", scene.sky_colour, context))
    {
        return false;
    }

    // Finally do the render
    if (!light_shader_->Render(light_buffer_->index_count(), context))
    {
        return false;
    }
    return true;
}

bool Deferred::RenderComposite(const Scene& scene, RenderContext& context)
{
    // TODO: TEMP DEBUG CODE. CLEAN THIS UP!!!
    static bool init = true;
    static const console::Variable* target = nullptr;
    static std::unique_ptr<Sprite> target_sprite(new Sprite("blons:none", context));
    static std::unique_ptr<Sprite> alt_target(new Sprite("blons:none", context));
    if (init)
    {
        console::RegisterVariable("gfx:target", 0);
        target = console::var("gfx:target");
        init = false;
        alt_target->set_pos(1380, 660, 200, 200);
        alt_target->set_subtexture(0, 0, 16, -16);
    }
    target_sprite->set_pos(0, 0, perspective_.width, perspective_.height);
    target_sprite->set_subtexture(0, 0, 16, -16);
    const TextureResource* screen_texture;
    const TextureResource* alt_screen_texture;
    switch (target->to<int>())
    {
    case 1:
        screen_texture = geometry_->output(stage::Geometry::ALBEDO);
        break;
    case 2:
        screen_texture = geometry_->output(stage::Geometry::NORMAL);
        break;
    case 3:
        screen_texture = geometry_->output(stage::Geometry::DEBUG);
        break;
    case 4:
        screen_texture = geometry_->output(stage::Geometry::DEPTH);
        break;
    case 5:
        screen_texture = shadow_->output(stage::Shadow::LIGHT_DEPTH);
        break;
    case 6:
        screen_texture = shadow_->output(stage::Shadow::DIRECT_LIGHT);
        break;
    case 7:
        screen_texture = lightprobe_->output(stage::Lightprobe::PROBE_ALBEDO);
        target_sprite->set_pos(200, 0, perspective_.width / 8, perspective_.height);
        break;
    case 8:
        screen_texture = lightprobe_->output(stage::Lightprobe::PROBE_UV);
        target_sprite->set_pos(200, 0, perspective_.width / 8, perspective_.height);
        break;
    case 9:
        screen_texture = lightprobe_->output(stage::Lightprobe::LIGHT_MAP_LOOKUP_POS);
        break;
    case 10:
        screen_texture = lightprobe_->output(stage::Lightprobe::LIGHT_MAP_LOOKUP_NORMAL);
        break;
    case 11:
        screen_texture = lightprobe_->output(stage::Lightprobe::DIRECT_LIGHT_MAP);
        break;
    case 12:
        screen_texture = lightprobe_->output(stage::Lightprobe::INDIRECT_LIGHT_MAP);
        break;
    case 13:
        screen_texture = lightprobe_->output(stage::Lightprobe::PROBE);
        target_sprite->set_pos(200, 0, perspective_.width / 8, perspective_.height);
        break;
    case 14:
        screen_texture = lightprobe_->output(stage::Lightprobe::COEFFICIENTS);
        target_sprite->set_pos(200, 0, perspective_.width / 8, perspective_.height);
        break;
    case 15:
        screen_texture = lightprobe_->output(stage::Lightprobe::INDIRECT_LIGHT);
        break;
    case 0:
    default:
        screen_texture = light_buffer_->textures()[0];
        break;
    }
    alt_screen_texture = geometry_->output(stage::Geometry::ALBEDO);

    // Needed so sprites can render over themselves
    context->SetDepthTesting(false);

    // Push the full screen quad used to render FBO
    target_sprite->Render(context);

    // Set the inputs
    if (!composite_shader_->SetInput("proj_matrix", ortho_matrix_, context) ||
        !composite_shader_->SetInput("sprite", screen_texture, context))
    {
        return false;
    }

    if (!composite_shader_->Render(target_sprite->index_count(), context))
    {
        return false;
    }

    // Push the mini screen quad used to render alt FBO
    alt_target->Render(context);

    // Set the inputs
    if (!composite_shader_->SetInput("proj_matrix", ortho_matrix_, context) ||
        !composite_shader_->SetInput("sprite", alt_screen_texture, context))
    {
        return false;
    }

    if (!composite_shader_->Render(alt_target->index_count(), context))
    {
        return false;
    }
    return true;
}
} // namespace pipeline
} // namespace blons