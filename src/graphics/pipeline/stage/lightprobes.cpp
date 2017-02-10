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
namespace
{
Vector3 FaceRotation(int face)
{
    units::world pitch = 0.0f;
    units::world yaw = 0.0f;
    switch (face)
    {
    case 0: // -Z
        break;
    case 1: // +X
        yaw = -kPi / 2.0f;
        break;
    case 2: // +Z
        yaw = kPi;
        break;
    case 3: // -X
        yaw = kPi / 2.0f;
        break;
    case 4: // +Y
        pitch = kPi / 2.0f;
        break;
    case 5: // -Y
        pitch = -kPi / 2.0f;
        break;
    default:
        throw "Impossible case statment reached during face selection";
    }
    return Vector3(pitch, yaw, 0.0f);
}
} // namespace

LightProbes::LightProbes()
{
    // Generate sponza probes, ofc we have to make this scene independant at some point...
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            for (int z = 0; z < 3; z++)
            {
                Probe probe { static_cast<int>(probes_.size()), Vector3(-14.0f + x * 4.0f, y * 5.0f + 2.0f, z * 5.0f - 5.0f) };
                probes_.push_back(probe);
            }
        }
    }
    for (int x = 0; x < 6; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            Probe probe { static_cast<int>(probes_.size()), Vector3(-10.0f + x * 4.0f, y * 3.0f + 11.0f, 0) };
            probes_.push_back(probe);
        }
    }
    // Initialize shader buffer to fit all probes in
    probe_shader_data_.reset(new ShaderData<LightProbes::Probe>(nullptr, probes_.size()));

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

    log::Debug("Baking environment maps... ");
    Timer bake_stats;
    // G-Buffer env map generation
    for (const auto& probe : probes_)
    {
        cube_view.set_pos(probe.pos.x, probe.pos.y, probe.pos.z);
        for (int face = 0; face < 6; face++)
        {
            Vector3 rot = FaceRotation(face);
            cube_view.set_rot(rot.x, rot.y, rot.z);
            context->SetViewport(face * kProbeMapSize, static_cast<units::pixel>(probe.id) * kProbeMapSize, kProbeMapSize, kProbeMapSize);
            for (const auto& m : scene.models)
            {
                m->Render();
                environment_map_shader_->SetInput("mvp_matrix", m->world_matrix() * cube_view.view_matrix() * cube_face_projection);
                environment_map_shader_->SetInput("normal_matrix", MatrixTranspose(MatrixInverse(m->world_matrix())));
                environment_map_shader_->SetInput("albedo", m->albedo(), 0);
                environment_map_shader_->SetInput("normal", m->normal(), 1);
                environment_map_shader_->Render(m->index_count());
            }
        }
    }
    log::Debug("[%ims]\n", bake_stats.ms());
    log::Debug("Baking sky visibility... ");
    bake_stats.start();

    // TODO: Move this to a compute shader
    // Sky visibility SH
    auto tex = context->GetTextureData(environment_maps_->textures()[0]);
    std::size_t pixel_size = tex.bits_per_pixel() / 8;
    for (const auto& probe : probes_)
    {
        SHCoeffs3 sh_sky_visibility;
        float sh_total_weight = 0.0f;
        for (int face = 0; face < 6; face++)
        {
            Vector3 rot = FaceRotation(face);
            cube_view.set_pos(0, 0, 0);
            // Since a view matrix rotates things in the opposite of the direction given
            // We reverse the pitch and yaw values
            cube_view.set_rot(-rot.x, -rot.y, rot.z);
            Matrix rotation_matrix = cube_view.view_matrix();

            for (int x = 0; x < kProbeMapSize; x++)
            {
                for (int y = 0; y < kProbeMapSize; y++)
                {
                    Vector2 uv;
                    uv.x = (static_cast<units::world>(x) + 0.5f) / static_cast<units::world>(kProbeMapSize) * 2.0f - 1.0f;
                    uv.y = (static_cast<units::world>(y) + 0.5f) / static_cast<units::world>(kProbeMapSize) * 2.0f - 1.0f;

                    Vector3 normal(uv.x, uv.y, -1.0f);
                    normal *= rotation_matrix;
                    normal = VectorNormalize(normal);

                    int px = x + face * kProbeMapSize;
                    int py = y + probe.id * kProbeMapSize;

                    // Retrieve sky alpha value from texture and normalize
                    auto sky_visibility = static_cast<units::world>(tex.pixels.get()[(px + py * tex.width) * pixel_size + 3]) / 255.0f;
                    // Weight texels contribution by its solid angle on the sphere
                    units::world sh_weight_intermediate = 1.0f + uv.x * uv.x + uv.y * uv.y;
                    units::world sh_texel_weight = 4.0f / (sqrt(sh_weight_intermediate) * sh_weight_intermediate);
                    // Add sample to SH coefficients
                    sh_sky_visibility += SHProjectDirection3(normal) * sky_visibility * sh_texel_weight;
                    // Build up averaging sums
                    sh_total_weight += sh_texel_weight;
                }
            }
        }
        // Normalize to surface area of unit sphere
        sh_sky_visibility *= 4.0f * kPi / sh_total_weight;
        probes_[probe.id].sh_sky_visibility = sh_sky_visibility;
    }
    log::Debug("[%ims]\n", bake_stats.ms());
    environment_maps_->Unbind();
    // Update shader buffer with generated probe data
    probe_shader_data_->set_value(probes_.data());
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

const std::vector<LightProbes::Probe>& LightProbes::probes() const
{
    return probes_;
}

const ShaderDataResource* LightProbes::probe_shader_data() const
{
    return probe_shader_data_->data();
}
} // namespace stage
} // namespace pipeline
} // namespace blons
