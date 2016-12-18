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

#include <blons/graphics/pipeline/stage/lightprobes.h>

// Public Includes
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
LightProbes::LightProbes()
{
    // Generate sponza probes, ofc we have to make this scene independant at some point...
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int z = 0; z < 3; z++)
            {
                Probe probe { probes_.size(), Vector3(-14.0f + x * 4.0f, y * 5.0f + 2.0f, z * 5.0f - 5.0f) };
                probes_.push_back(probe);
            }
        }
    }
    for (int x = 0; x < 6; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            Probe probe { probes_.size(), Vector3(-10.0f + x * 4.0f, y * 3.0f + 11.0f, 0) };
            probes_.push_back(probe);
        }
    }

    ShaderAttributeList env_map_inputs;
    env_map_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    env_map_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    env_map_inputs.push_back(ShaderAttribute(ShaderAttributeIndex::NORMAL, "input_norm"));
    env_map_inputs.push_back(ShaderAttribute(TANGENT, "input_tan"));
    env_map_inputs.push_back(ShaderAttribute(BITANGENT, "input_bitan"));
    environment_map_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/probe-env-map.frag.glsl", env_map_inputs));

    environment_maps_.reset(new Framebuffer(kProbeMapSize * 6, kProbeMapSize * static_cast<units::pixel>(probes_.size()),
                                            { { TextureType::R8G8B8A8, TextureType::RAW, TextureType::NEAREST, TextureType::CLAMP },   // albedo + sky vis
                                              { TextureType::R8G8B8,   TextureType::RAW, TextureType::NEAREST, TextureType::CLAMP } }, // normal
                                            true));
}

void LightProbes::BakeRadianceTransfer(const Scene& scene)
{
    // Hard coded distance clipping as graphics option values are tuned for performance
    Matrix cube_face_projection = MatrixPerspective(kPi / 2.0f, 1.0f, 0.1f, 10000.0f);
    Camera cube_view;
    auto context = render::context();

    environment_maps_->Bind(Vector4(0, 1, 0, 1));
    context->SetDepthTesting(true);
    context->SetBlendMode(BlendMode::OVERWRITE);
    auto render_scene = [&]()
    {
        for (const auto& m : scene.models)
        {
            m->Render();
            environment_map_shader_->SetInput("mvp_matrix", m->world_matrix() * cube_view.view_matrix() * cube_face_projection);
            environment_map_shader_->SetInput("albedo", m->albedo(), 0);
            environment_map_shader_->SetInput("normal", m->normal(), 1);
            environment_map_shader_->Render(m->index_count());
        }
    };

    for (const auto& probe : probes_)
    {
        cube_view.set_pos(probe.pos.x, probe.pos.y, probe.pos.z);

        cube_view.set_rot(0, 0, 0);
        context->SetViewport(0 * kProbeMapSize, static_cast<units::pixel>(probe.id) * kProbeMapSize, kProbeMapSize, kProbeMapSize);
        render_scene();

        cube_view.set_rot(0, -kPi / 2.0f, 0);
        context->SetViewport(1 * kProbeMapSize, static_cast<units::pixel>(probe.id) * kProbeMapSize, kProbeMapSize, kProbeMapSize);
        render_scene();

        cube_view.set_rot(0, kPi, 0);
        context->SetViewport(2 * kProbeMapSize, static_cast<units::pixel>(probe.id) * kProbeMapSize, kProbeMapSize, kProbeMapSize);
        render_scene();

        cube_view.set_rot(0, kPi / 2.0f, 0);
        context->SetViewport(3 * kProbeMapSize, static_cast<units::pixel>(probe.id) * kProbeMapSize, kProbeMapSize, kProbeMapSize);
        render_scene();

        cube_view.set_rot(kPi / 2.0f, 0, 0);
        context->SetViewport(4 * kProbeMapSize, static_cast<units::pixel>(probe.id) * kProbeMapSize, kProbeMapSize, kProbeMapSize);
        render_scene();

        cube_view.set_rot(-kPi / 2.0f, 0, 0);
        context->SetViewport(5 * kProbeMapSize, static_cast<units::pixel>(probe.id) * kProbeMapSize, kProbeMapSize, kProbeMapSize);
        render_scene();
    }
    environment_maps_->Unbind();
}

const TextureResource* LightProbes::output(Output buffer) const
{
    switch (buffer)
    {
    case ENV_MAPS:
        return environment_maps_->textures()[0];
        break;
    default:
        return nullptr;
    }
}
} // namespace stage
} // namespace pipeline
} // namespace blons
