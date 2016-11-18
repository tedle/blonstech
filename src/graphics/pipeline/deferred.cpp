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
#include <blons/graphics/pipeline/stage/lighting.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>
#include <blons/graphics/sprite.h>

namespace blons
{
namespace pipeline
{
Deferred::Deferred(Client::Info screen, float fov, float screen_near, float screen_far)
{
    output_ = FINAL;
    alt_output_ = ALBEDO;
    Reload(screen, fov, screen_near, screen_far);
}

Deferred::~Deferred() {}

bool Deferred::Init()
{
    // Projection matrix (3D space->2D screen)
    float screen_aspect = static_cast<float>(perspective_.width) / static_cast<float>(perspective_.height);
    proj_matrix_ = MatrixPerspective(perspective_.fov, screen_aspect, perspective_.screen_near, perspective_.screen_far);

    // Ortho projection matrix (for 2d stuff, shadow maps, etc)
    ortho_matrix_ = MatrixOrthographic(0, units::pixel_to_subpixel(perspective_.width), units::pixel_to_subpixel(perspective_.height), 0,
                                       perspective_.screen_near, perspective_.screen_far);

    // Pipeline setup
    geometry_.reset(new stage::Geometry(perspective_));
    shadow_.reset(new stage::Shadow(perspective_));
    lightprobe_.reset(new stage::Lightprobe(perspective_));
    lighting_.reset(new stage::Lighting(perspective_));

    // Shaders
    ShaderAttributeList composite_inputs;
    composite_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    composite_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    composite_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/sprite.frag.glsl", composite_inputs));

    if (composite_shader_ == nullptr)
    {
        return false;
    }

    output_sprite_.reset(new Sprite("blons:none"));
    alt_output_sprite_.reset(new Sprite("blons:none"));

    return true;
}

void Deferred::Reload(Client::Info screen, float fov, float screen_near, float screen_far)
{
    perspective_.width = screen.width;
    perspective_.height = screen.height;
    perspective_.screen_near = screen_near;
    perspective_.screen_far = screen_far;
    perspective_.fov = fov;

    Init();
}

bool Deferred::Render(const Scene& scene)
{
    return Render(scene, nullptr);
}

bool Deferred::Render(const Scene& scene, Framebuffer* output_buffer)
{
    // TODO: Support more scene lights
    assert(scene.lights.size() == 1);
    Light* sun = scene.lights[0];

    // Calculates view_matrix from scratch, so we cache it
    Matrix view_matrix = scene.view.view_matrix();
    Matrix light_vp_matrix = sun->ViewFrustum(view_matrix * proj_matrix_, perspective_.screen_far);

    // Render all of the geometry and accompanying info (normal, depth, etc)
    if (!geometry_->Render(scene, view_matrix, proj_matrix_))
    {
        return false;
    }

    // Render all of the geometry and get their depth from the light's point of view
    // Then render a shadow map from the depth information
    if (!shadow_->Render(scene, *geometry_, view_matrix, proj_matrix_, light_vp_matrix, ortho_matrix_))
    {
        return false;
    }

    // Builds a direct light map and then bounce lighting
    if (!lightprobe_->Render(scene, *geometry_, *shadow_, perspective_,
                             view_matrix, proj_matrix_, light_vp_matrix))
    {
        return false;
    }

    if (!lighting_->Render(scene, *geometry_, *shadow_, *lightprobe_, view_matrix, proj_matrix_, ortho_matrix_))
    {
        return false;
    }

    // Bind the output buffer
    if (output_buffer != nullptr)
    {
        output_buffer->Bind();
    }
    else
    {
        render::context()->BindFramebuffer(nullptr);
    }

    // Render the final composite of the geometry and lighting passes
    if (!RenderComposite(scene))
    {
        return false;
    }

    return true;
}

bool Deferred::BuildLighting(const Scene& scene)
{
    return lightprobe_->BuildLighting(scene);
}

void Deferred::set_output(Output output, Output alt_output)
{
    output_ = output;
    alt_output_ = alt_output;
}

bool Deferred::RenderComposite(const Scene& scene)
{
    output_sprite_->set_pos(0, 0, perspective_.width, perspective_.height);
    output_sprite_->set_subtexture(0, 16, 16, -16);
    alt_output_sprite_->set_pos(1380, 660, 200, 200);
    alt_output_sprite_->set_subtexture(0, 16, 16, -16);

    auto output_texture = [&](Output output, Sprite* output_sprite) -> const TextureResource*
    {
        switch (output)
        {
        case ALBEDO:
            return geometry_->output(stage::Geometry::ALBEDO);
            break;
        case NORMAL:
            return geometry_->output(stage::Geometry::NORMAL);
            break;
        case DEBUG:
            return geometry_->output(stage::Geometry::DEBUG);
            break;
        case G_DEPTH:
            return geometry_->output(stage::Geometry::DEPTH);
            break;
        case LIGHT_DEPTH:
            return shadow_->output(stage::Shadow::LIGHT_DEPTH);
            break;
        case DIRECT_LIGHT:
            return shadow_->output(stage::Shadow::DIRECT_LIGHT);
            break;
        case PROBE_ALBEDO:
            output_sprite->set_pos(200, 0, perspective_.width / 8, perspective_.height);
            return lightprobe_->output(stage::Lightprobe::PROBE_ALBEDO);
            break;
        case PROBE_UV:
            output_sprite->set_pos(200, 0, perspective_.width / 8, perspective_.height);
            return lightprobe_->output(stage::Lightprobe::PROBE_UV);
            break;
        case LIGHT_MAP_LOOKUP_POS:
            return lightprobe_->output(stage::Lightprobe::LIGHT_MAP_LOOKUP_POS);
            break;
        case LIGHT_MAP_LOOKUP_NORMAL:
            return lightprobe_->output(stage::Lightprobe::LIGHT_MAP_LOOKUP_NORMAL);
            break;
        case DIRECT_LIGHT_MAP:
            return lightprobe_->output(stage::Lightprobe::DIRECT_LIGHT_MAP);
            break;
        case INDIRECT_LIGHT_MAP:
            return lightprobe_->output(stage::Lightprobe::INDIRECT_LIGHT_MAP);
            break;
        case PROBE:
            output_sprite->set_pos(200, 0, perspective_.width / 8, perspective_.height);
            return lightprobe_->output(stage::Lightprobe::PROBE);
            break;
        case PROBE_COEFFICIENTS:
            output_sprite->set_pos(200, 0, perspective_.width / 8, perspective_.height);
            return lightprobe_->output(stage::Lightprobe::COEFFICIENTS);
            break;
        case INDIRECT_LIGHT:
            return lightprobe_->output(stage::Lightprobe::INDIRECT_LIGHT);
            break;
        case NONE:
            return nullptr;
            break;
        case FINAL:
        default:
            return lighting_->output(stage::Lighting::LIGHT);
            break;
        }
    };
    // Needed so sprites can render over themselves
    render::context()->SetDepthTesting(false);

    const TextureResource* texture;
    texture = output_texture(output_, output_sprite_.get());
    if (texture != nullptr)
    {
        // Push the full screen quad used to render FBO
        output_sprite_->Render();

        // Set the inputs
        if (!composite_shader_->SetInput("proj_matrix", ortho_matrix_) ||
            !composite_shader_->SetInput("sprite", texture))
        {
            return false;
        }

        if (!composite_shader_->Render(output_sprite_->index_count()))
        {
            return false;
        }
    }

    texture = output_texture(alt_output_, alt_output_sprite_.get());
    if (texture != nullptr)
    {
        // Push the mini screen quad used to render alt FBO
        alt_output_sprite_->Render();

        // Set the inputs
        if (!composite_shader_->SetInput("proj_matrix", ortho_matrix_) ||
            !composite_shader_->SetInput("sprite", texture))
        {
            return false;
        }

        if (!composite_shader_->Render(alt_output_sprite_->index_count()))
        {
            return false;
        }
    }
    return true;
}
} // namespace pipeline
} // namespace blons