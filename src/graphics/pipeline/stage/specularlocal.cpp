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

#include <blons/graphics/pipeline/stage/specularlocal.h>

// Public Includes
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/drawbatcher.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
namespace temp
{
std::vector<Vector3> GenerateOldSponzaProbePositions()
{
    return { Vector3(0.0f, 5.0f, 0.0f) };
}
} // namespace temp
namespace
{
std::array<Matrix, 6> GenerateViewProjMatrices(Vector3 position, bool depth_buffer_zero_to_one)
{
    std::array<Matrix, 6> matrices;
    Camera view;
    view.set_pos(position.x, position.y, position.z);
    Matrix cube_projection = MatrixPerspective(kPi / 2.0f, 1.0f, 0.1f, 1000.0f, depth_buffer_zero_to_one);
    for (const auto& face : { POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z })
    {
        Vector3 rot = AxisRotationPitchYawRoll(face);
        view.set_rot(rot.x, rot.y, rot.z);
        matrices[face] = view.view_matrix() * cube_projection;
    }
    return matrices;
}
} // namespace

SpecularLocal::SpecularLocal()
{
    const auto probe_positions = temp::GenerateOldSponzaProbePositions();
    for (const auto& pos : probe_positions)
    {
        SpecularProbe probe;
        probe.pos = pos;
        PixelDataCubemap buffer;
        buffer.width = kSpecularProbeMapSize;
        buffer.height = kSpecularProbeMapSize;
        // G-buffer albedo
        buffer.type = TextureType(TextureType::R8G8B8, TextureType::RAW, TextureType::LINEAR);
        probe.g_buffer.albedo.reset(new TextureCubemap(buffer));
        // G-buffer normal (16-bits needed for accuracy)
        buffer.type.format = TextureType::R16G16B16_UNORM;
        probe.g_buffer.normal.reset(new TextureCubemap(buffer));
        // G-buffer depth
        buffer.type.format = TextureType::DEPTH;
        probe.g_buffer.depth.reset(new TextureCubemap(buffer));
        // HDR Relighting buffer, with mipmaps for roughness
        buffer.type.format = TextureType::R16G16B16A16_FLOAT;
        buffer.type.compression = TextureType::AUTO;
        probe.environment.reset(new TextureCubemap(buffer));
        // Add it to the list
        probes_.push_back(std::move(probe));
    }

    relight_shader_.reset(new ComputeShader({ { COMPUTE, "shaders/specular-probe-relight.comp.glsl"} }));
}

void SpecularLocal::BakeRadianceTransfer(const Scene& scene)
{
    auto context = render::context();
    // Settings for rendering a G-buffer
    context->SetDepthTesting(true);
    context->SetBlendMode(BlendMode::OVERWRITE);

    // Compile the G-buffer shader
    ShaderAttributeList env_map_inputs = { { POS, "input_pos" },
                                           { TEX, "input_uv" },
                                           { ShaderAttributeIndex::NORMAL, "input_norm" },
                                           { TANGENT, "input_tan" },
                                           { BITANGENT, "input_bitan" } };
    ShaderSourceList env_map_source = { { VERTEX, "shaders/specular-probe-env-map.vert.glsl" },
                                        { GEOMETRY, "shaders/specular-probe-env-map.geom.glsl" },
                                        { PIXEL, "shaders/mesh.frag.glsl" } };
    auto env_map_shader = std::make_unique<Shader>(env_map_source, env_map_inputs);
    // Create an empty framebuffer that we'll attach our textures to
    auto fbo = std::make_unique<Framebuffer>(kSpecularProbeMapSize, kSpecularProbeMapSize, std::vector<TextureType>(), false);

    for (const auto& probe : probes_)
    {
        // Create a list of 6 view-proj matrices to render the scene from for the cubemap
        std::array<Matrix, 6> vp_matrices = GenerateViewProjMatrices(probe.pos, context->IsDepthBufferRangeZeroToOne());
        // Bind the cubemap's G-buffer textures to the framebuffer
        std::vector<const TextureResource*> textures = { probe.g_buffer.albedo->texture(), probe.g_buffer.normal->texture() };
        fbo->BindColourTextures(textures);
        fbo->BindDepthTexture(probe.g_buffer.depth->texture());
        // Clears the buffers
        fbo->Bind();
        // Render the scene with each model instanced for each cubeface (instancing done in shader)
        for (const auto& m : scene.models)
        {
            m->Render();
            if (!env_map_shader->SetInput("model_matrix", m->world_matrix()) ||
                !env_map_shader->SetInput("vp_matrices", vp_matrices.data(), 6) ||
                !env_map_shader->SetInput("normal_matrix", MatrixTranspose(MatrixInverse(m->world_matrix()))) ||
                !env_map_shader->SetInput("albedo", m->albedo(), 0) ||
                !env_map_shader->SetInput("normal", m->normal(), 1))
            {
                throw "Failed to set uniforms for specular probe init shader";
            }
            env_map_shader->Render(m->index_count());
        }
    }
    // Make sure our textures don't get overwritten later
    fbo->Unbind();
}

bool SpecularLocal::Relight(const Scene& scene, const Shadow& shadow, const IrradianceVolume& irradiance, Matrix light_vp_matrix)
{
    auto context = render::context();
    // Can be removed when we support more lights
    assert(scene.lights.size() == 1);
    Light* sun = scene.lights[0];
    // Inverted view-proj matrices to find UV space positions of cubemaps
    std::array<Matrix, 6> inv_direction_matrices = GenerateViewProjMatrices(Vector3(0.0f), context->IsDepthBufferRangeZeroToOne());
    std::transform(inv_direction_matrices.begin(), inv_direction_matrices.end(), inv_direction_matrices.begin(), [](const auto& mat) { return MatrixInverse(mat); });
    for (const auto& probe : probes_)
    {
        // Inverted view-proj matrices to find world space positions from G-buffer
        std::array<Matrix, 6> inv_vp_matrices = GenerateViewProjMatrices(probe.pos, context->IsDepthBufferRangeZeroToOne());
        std::transform(inv_vp_matrices.begin(), inv_vp_matrices.end(), inv_vp_matrices.begin(), [](const auto& mat) { return MatrixInverse(mat); });
        if (!relight_shader_->SetInput("inv_direction_matrices", inv_direction_matrices.data(), 6) ||
            !relight_shader_->SetInput("inv_vp_matrices", inv_vp_matrices.data(), 6) ||
            !relight_shader_->SetInput("light_vp_matrix", light_vp_matrix) ||
            !relight_shader_->SetInput("albedo", probe.g_buffer.albedo->texture(), 0) ||
            !relight_shader_->SetInput("normal", probe.g_buffer.normal->texture(), 1) ||
            !relight_shader_->SetInput("depth", probe.g_buffer.depth->texture(), 2) ||
            !relight_shader_->SetInput("light_depth", shadow.output(Shadow::LIGHT_DEPTH), 3) ||
            !relight_shader_->SetInput("sun.dir", sun->direction()) ||
            !relight_shader_->SetInput("sun.colour", sun->colour()) ||
            !relight_shader_->SetInput("sun.luminance", sun->luminance()) ||
            !relight_shader_->SetOutput("env_map", probe.environment->texture(), 0, 0))
        {
            return false;
        }
        // One invocation for each pixel for each cubemap layer
        relight_shader_->Run(kSpecularProbeMapSize, kSpecularProbeMapSize, 6);
    }
    return true;
}

const TextureResource* SpecularLocal::output(Output buffer, std::size_t probe_id) const
{
    if (probe_id > probes_.size())
    {
        throw "Out of bounds reflection probe access";
    }
    switch (buffer)
    {
    case ALBEDO:
        return probes_[probe_id].g_buffer.albedo->texture();
    case NORMAL:
        return probes_[probe_id].g_buffer.normal->texture();
    case DEPTH:
        return probes_[probe_id].g_buffer.depth->texture();
    case LIGHT:
        return probes_[probe_id].environment->texture();
    default:
        throw "Non-existant buffer access attempted";
    }
    return nullptr;
}
} // namespace stage
} // namespace pipeline
} // namespace blons