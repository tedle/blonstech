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
        // HDR Relighting buffer, with mipmaps for pre-filtered importance sampling
        buffer.type.format = TextureType::R16G16B16A16_FLOAT;
        buffer.type.compression = TextureType::AUTO;
        probe.environment.reset(new TextureCubemap(buffer));
        // HDR LD term for ambient specular, with mipmaps for varying roughness
        buffer.type.format = TextureType::R16G16B16A16_FLOAT;
        buffer.type.compression = TextureType::RAW;
        probe.ld_term.reset(new TextureCubemap(buffer));
        render::context()->SetTextureMipmapRange(probe.ld_term->mutable_texture().get(), 0, kSpecularProbeMipLevels);
        render::context()->MakeTextureMipmaps(probe.ld_term->mutable_texture().get());
        // Add it to the list
        probes_.push_back(std::move(probe));
    }

    // Compile the relighting shader
    ShaderAttributeList env_map_inputs = { { POS, "input_pos" },
                                           { TEX, "input_uv" } };
    ShaderSourceList env_map_source = { { VERTEX, "shaders/specular-probe-relight.vert.glsl" },
                                        { GEOMETRY, "shaders/specular-probe-relight.geom.glsl" },
                                        { PIXEL, "shaders/specular-probe-relight.frag.glsl" } };
    ShaderSourceList ld_term_source = { { VERTEX, "shaders/specular-probe-relight.vert.glsl" },
                                        { GEOMETRY, "shaders/specular-probe-relight.geom.glsl" },
                                        { PIXEL, "shaders/specular-probe-distribution.frag.glsl" } };
    relight_shader_.reset(new Shader(env_map_source, env_map_inputs));
    relight_distribution_shader_.reset(new Shader(ld_term_source, env_map_inputs));
    relight_buffer_.reset(new Framebuffer(kSpecularProbeMapSize, kSpecularProbeMapSize, 0, false));
    // Inverted view-proj matrices to find UV space positions of cubemaps
    std::array<Matrix, 6> inv_direction_matrices = GenerateViewProjMatrices(Vector3(0.0f), render::context()->IsDepthBufferRangeZeroToOne());
    std::transform(inv_direction_matrices.begin(), inv_direction_matrices.end(), inv_direction_matrices.begin(), [](const auto& mat) { return MatrixInverse(mat); });
    if (!relight_shader_->SetInput("inv_direction_matrices", inv_direction_matrices.data(), 6) ||
        !relight_shader_->SetInput("proj_matrix", MatrixOrthographic(0.0f, static_cast<units::world>(kSpecularProbeMapSize),
                                                                     static_cast<units::world>(kSpecularProbeMapSize), 0.0f,
                                                                     kScreenNear, kScreenFar)))
    {
        throw "Failed to initialize constant shader settings for specular probe relight";
    }
    if (!relight_distribution_shader_->SetInput("inv_direction_matrices", inv_direction_matrices.data(), 6) ||
        !relight_distribution_shader_->SetInput("proj_matrix", MatrixOrthographic(0.0f, static_cast<units::world>(kSpecularProbeMapSize),
                                                                                  static_cast<units::world>(kSpecularProbeMapSize), 0.0f,
                                                                                  kScreenNear, kScreenFar)))
    {
        throw "Failed to initialize constant shader settings for specular probe distribution";
    }
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
    auto fbo = std::make_unique<Framebuffer>(kSpecularProbeMapSize, kSpecularProbeMapSize, 0, false);

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

    // TODO: WHEN WE'RE BAKING DFG, USE 4-CHANNEL TEXTURE WITH 2 RESERVED FOR SINGLE/MULTI TERMS OF DIFFUSE GGX
    // or make a separate 2 channel dfg texture owned by light sector might be more sensible. but thats an extra texture fetch wauhg
}

bool SpecularLocal::Relight(const Scene& scene, const Shadow& shadow, const IrradianceVolume& irradiance, Matrix light_vp_matrix)
{
    auto context = render::context();
    context->SetDepthTesting(false);
    context->SetBlendMode(BlendMode::OVERWRITE);
    // Can be removed when we support more lights
    assert(scene.lights.size() == 1);
    Light* sun = scene.lights[0];

    if (!relight_shader_->SetInput("light_vp_matrix", light_vp_matrix) ||
        !relight_shader_->SetInput("light_depth", shadow.output(Shadow::LIGHT_DEPTH), 3) ||
        !relight_shader_->SetInput("sun.dir", sun->direction()) ||
        !relight_shader_->SetInput("sun.colour", sun->colour()) ||
        !relight_shader_->SetInput("sun.luminance", sun->luminance()) ||
        !relight_shader_->SetInput("sky_luminance", scene.sky_luminance) ||
        !relight_shader_->SetInput("sh_sky_colour.r", scene.sky_box.r.coeffs, 9) ||
        !relight_shader_->SetInput("sh_sky_colour.g", scene.sky_box.g.coeffs, 9) ||
        !relight_shader_->SetInput("sh_sky_colour.b", scene.sky_box.b.coeffs, 9) ||
        !relight_shader_->SetInput("inv_irradiance_matrix", MatrixInverse(irradiance.world_matrix())) ||
        !relight_shader_->SetInput("irradiance_volume_px", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PX), 4) ||
        !relight_shader_->SetInput("irradiance_volume_nx", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_NX), 5) ||
        !relight_shader_->SetInput("irradiance_volume_py", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PY), 6) ||
        !relight_shader_->SetInput("irradiance_volume_ny", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_NY), 7) ||
        !relight_shader_->SetInput("irradiance_volume_pz", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_PZ), 8) ||
        !relight_shader_->SetInput("irradiance_volume_nz", irradiance.output(IrradianceVolume::IRRADIANCE_VOLUME_NZ), 9))
    {
        return false;
    }

    // Relight the base mip level of all lit environment maps
    relight_buffer_->Bind();
    for (const auto& probe : probes_)
    {
        // Inverted view-proj matrices to find world space positions from G-buffer
        std::array<Matrix, 6> inv_vp_matrices = GenerateViewProjMatrices(probe.pos, context->IsDepthBufferRangeZeroToOne());
        std::transform(inv_vp_matrices.begin(), inv_vp_matrices.end(), inv_vp_matrices.begin(), [](const auto& mat) { return MatrixInverse(mat); });
        if (!relight_shader_->SetInput("inv_vp_matrices", inv_vp_matrices.data(), 6) ||
            !relight_shader_->SetInput("albedo", probe.g_buffer.albedo->texture(), 0) ||
            !relight_shader_->SetInput("normal", probe.g_buffer.normal->texture(), 1) ||
            !relight_shader_->SetInput("depth", probe.g_buffer.depth->texture(), 2))
        {
            relight_buffer_->Unbind();
            return false;
        }
        relight_buffer_->BindColourTextures({ probe.environment->texture(), probe.ld_term->texture() });
        relight_buffer_->Render();
        relight_shader_->Render(relight_buffer_->index_count());
    }
    // Propogate the relighting to the lower mip levels for pre-filtered importance sampling
    // Done in a separate loop to mitigate state changes in the rendering context
    for (const auto& probe : probes_)
    {
        context->MakeTextureMipmaps(probe.environment->mutable_texture().get());
    }
    // Approximate the specular lighting distribution for each mipmaps roughness level
    for (const auto& probe : probes_)
    {
        if (!relight_distribution_shader_->SetInput("environment_map", probe.environment->texture(), 0) ||
            !relight_distribution_shader_->SetInput("max_mip_level", kSpecularProbeMipLevels) ||
            !relight_distribution_shader_->SetInput("base_texture_size", kSpecularProbeMapSize))
        {
            relight_buffer_->Unbind();
            return false;
        }
        for (int mip_level = 1; mip_level <= kSpecularProbeMipLevels; mip_level++)
        {
            units::pixel resolution = kSpecularProbeMapSize >> mip_level;
            if (!relight_distribution_shader_->SetInput("mip_level", mip_level))
            {
                relight_buffer_->Unbind();
                return false;
            }
            context->SetViewport(0, 0, resolution, resolution);
            relight_buffer_->BindColourTextures({ probe.ld_term->texture() }, mip_level);
            relight_buffer_->Render();
            relight_distribution_shader_->Render(relight_buffer_->index_count());
        }
    }
    relight_buffer_->Unbind();
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
    case LD_TERM:
        return probes_[probe_id].ld_term->texture();
    default:
        throw "Non-existant buffer access attempted";
    }
    return nullptr;
}
} // namespace stage
} // namespace pipeline
} // namespace blons