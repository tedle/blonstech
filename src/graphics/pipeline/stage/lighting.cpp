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

#include <blons/graphics/pipeline/stage/lighting.h>

// Public Includes
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>
#include <blons/graphics/render/shaderdata.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace
{
// TODO: Replace with proper material system
auto cvar_roughness = console::RegisterVariable("mtl:roughness", 0.4f);
auto cvar_metalness = console::RegisterVariable("mtl:metalness", 0.0f);
} // namespace

Lighting::Lighting(Perspective perspective)
{
    // Shaders
    ShaderAttributeList light_inputs;
    light_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    light_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    light_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/light.frag.glsl", light_inputs));

    if (light_shader_ == nullptr)
    {
        throw "Failed to initialize lighting shaders";
    }

    // Framebuffers
    light_buffer_.reset(new Framebuffer(perspective.width, perspective.height, 1, false));
}

bool Lighting::Render(const Scene& scene, const Geometry& geometry, const Shadow& shadow, const IrradianceVolume& irradiance,
                      Matrix view_matrix, Matrix proj_matrix, Matrix ortho_matrix)
{
    auto context = render::context();
    // Can be removed when we support more lights
    assert(scene.lights.size() == 1);
    Light* sun = scene.lights[0];

    // Used to turn pixel fragments into world coordinates
    Matrix inv_proj_view = MatrixInverse(view_matrix * proj_matrix);

    // Bind the buffer to do all lighting calculations on
    light_buffer_->Bind();

    light_buffer_->Render();

    // Set the inputs
    if (!light_shader_->SetInput("proj_matrix", ortho_matrix) ||
        !light_shader_->SetInput("inv_vp_matrix", inv_proj_view) ||
        !light_shader_->SetInput("albedo", geometry.output(stage::Geometry::ALBEDO), 0) ||
        !light_shader_->SetInput("normal", geometry.output(stage::Geometry::NORMAL), 1) ||
        !light_shader_->SetInput("depth", geometry.output(stage::Geometry::DEPTH), 2) ||
        !light_shader_->SetInput("direct_light", shadow.output(stage::Shadow::DIRECT_LIGHT), 3) ||
        !light_shader_->SetInput("sun.dir", sun->direction()) ||
        !light_shader_->SetInput("sun.colour", sun->colour()) ||
        !light_shader_->SetInput("sun.luminance", sun->luminance()) ||
        !light_shader_->SetInput("sky_luminance", scene.sky_luminance) ||
        !light_shader_->SetInput("exposure", scene.view.exposure()) ||
        !light_shader_->SetInput("roughness", cvar_roughness->to<float>()) ||
        !light_shader_->SetInput("metalness", Vector3(cvar_metalness->to<float>())) ||
        !light_shader_->SetInput("sh_sky_colour.r", scene.sky_box.r.coeffs, 9) ||
        !light_shader_->SetInput("sh_sky_colour.g", scene.sky_box.g.coeffs, 9) ||
        !light_shader_->SetInput("sh_sky_colour.b", scene.sky_box.b.coeffs, 9) ||
        !light_shader_->SetInput("inv_irradiance_matrix", MatrixInverse(irradiance.world_matrix())) ||
        !light_shader_->SetInput("irradiance_volume_px", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PX), 4) ||
        !light_shader_->SetInput("irradiance_volume_nx", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_NX), 5) ||
        !light_shader_->SetInput("irradiance_volume_py", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PY), 6) ||
        !light_shader_->SetInput("irradiance_volume_ny", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_NY), 7) ||
        !light_shader_->SetInput("irradiance_volume_pz", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PZ), 8) ||
        !light_shader_->SetInput("irradiance_volume_nz", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_NZ), 9))
    {
        return false;
    }

    // Finally do the render
    if (!light_shader_->Render(light_buffer_->index_count()))
    {
        return false;
    }

    return true;
}

const TextureResource* Lighting::output(Output buffer) const
{
    switch (buffer)
    {
    case LIGHT:
        return light_buffer_->textures()[0];
        break;
    default:
        return nullptr;
    }
}
} // namespace stage
} // namespace pipeline
} // namespace blons