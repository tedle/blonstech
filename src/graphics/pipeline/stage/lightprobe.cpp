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

#include <blons/graphics/pipeline/stage/lightprobe.h>

// Public Includes
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/render/shader.h>
#include <blons/graphics/pipeline/stage/geometry.h>
#include <blons/graphics/pipeline/stage/shadow.h>

namespace blons
{
namespace pipeline
{
namespace stage
{
Lightprobe::Lightprobe(Perspective perspective, RenderContext& context)
{
    // Probe camera
    probe_view_.reset(new Camera);
    if (probe_view_ == nullptr)
    {
        throw "Failed to initialize probe camera";
    }

    light_map_ortho_matrix_ = MatrixOrthographic(0,
        static_cast<units::world>(kLightMapResolution),
        static_cast<units::world>(kLightMapResolution),
        0, 0, 1);

    GenerateProbes();

    // Probe projection matrix (cube map perspective)
    // 90 degree FOV to make seams between edges add up perfectly to 360
    // Cube map aspect ratio, of course, is 1
    probe_proj_matrix_ = MatrixPerspective(kPi / 2, 1.0f, perspective.screen_near, perspective.screen_far);
    // For when we do probe lookups and renders
    probe_ortho_matrix_ = MatrixOrthographic(0,
        static_cast<units::world>(6 * kProbeMapSize),
        static_cast<units::world>(probes_.size() * kProbeMapSize),
        0, 0, 1);

    // Shaders
    ShaderAttributeList direct_light_map_inputs;
    direct_light_map_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    direct_light_map_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    direct_light_map_shader_.reset(new Shader("shaders/direct-light-map.vert.glsl", "shaders/direct-light-map.frag.glsl", direct_light_map_inputs, context));

    ShaderAttributeList indirect_light_map_inputs;
    indirect_light_map_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    indirect_light_map_inputs.push_back(ShaderAttribute(TEX, "input_uv")); // Stores probe id
    indirect_light_map_inputs.push_back(ShaderAttribute(NORMAL, "input_norm")); // Stores probe position
    indirect_light_map_shader_.reset(new Shader("shaders/indirect-light-map.vert.glsl", "shaders/indirect-light-map.frag.glsl", indirect_light_map_inputs, context));

    ShaderAttributeList indirect_light_inputs;
    indirect_light_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    indirect_light_inputs.push_back(ShaderAttribute(TEX, "input_uv")); // Stores probe id
    indirect_light_inputs.push_back(ShaderAttribute(NORMAL, "input_norm")); // Stores probe position
    indirect_light_shader_.reset(new Shader("shaders/indirect-light.vert.glsl", "shaders/indirect-light.frag.glsl", indirect_light_inputs, context));

    ShaderAttributeList light_map_lookup_inputs;
    light_map_lookup_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    light_map_lookup_inputs.push_back(ShaderAttribute(LIGHT_TEX, "input_light_uv"));
    light_map_lookup_inputs.push_back(ShaderAttribute(NORMAL, "input_norm"));
    light_map_lookup_shader_.reset(new Shader("shaders/light-map-lookup.vert.glsl", "shaders/light-map-lookup.frag.glsl", light_map_lookup_inputs, context));

    ShaderAttributeList probe_map_inputs;
    probe_map_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    probe_map_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    probe_map_inputs.push_back(ShaderAttribute(LIGHT_TEX, "input_light_uv"));
    probe_map_shader_.reset(new Shader("shaders/probe-map.vert.glsl", "shaders/probe-map.frag.glsl", probe_map_inputs, context));

    ShaderAttributeList probe_map_clear_inputs;
    probe_map_clear_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    probe_map_clear_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/probe-map-clear.frag.glsl", probe_map_clear_inputs, context));

    ShaderAttributeList probe_inputs;
    probe_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    probe_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    probe_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/probe.frag.glsl", probe_inputs, context));

    ShaderAttributeList probe_coefficients_inputs;
    probe_coefficients_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    probe_coefficients_shader_.reset(new Shader("shaders/sh-coefficients.vert.glsl", "shaders/sh-coefficients.frag.glsl", probe_coefficients_inputs, context));

    if (indirect_light_shader_ == nullptr ||
        direct_light_map_shader_ == nullptr ||
        indirect_light_map_shader_ == nullptr ||
        light_map_lookup_shader_ == nullptr ||
        probe_map_shader_ == nullptr ||
        probe_map_clear_shader_ == nullptr ||
        probe_shader_ == nullptr ||
        probe_coefficients_shader_ == nullptr)
    {
        throw "Failed to initialize light probe shaders";
    }

    // Framebuffers
    indirect_light_buffer_.reset(new Framebuffer(perspective.width, perspective.height, { { TextureHint::R32G32B32A32, TextureHint::LINEAR } }, false, context));
    light_map_lookup_buffer_.reset(new Framebuffer(kLightMapResolution, kLightMapResolution, { { TextureHint::R32G32B32, TextureHint::NEAREST }, { TextureHint::R8G8B8, TextureHint::NEAREST } }, false, context));
    direct_light_map_accumulation_buffer_.reset(new Framebuffer(kLightMapResolution, kLightMapResolution, { { TextureHint::R32G32B32A32, TextureHint::LINEAR } }, false, context));
    indirect_light_map_accumulation_buffer_.reset(new Framebuffer(kLightMapResolution, kLightMapResolution, { { TextureHint::R32G32B32A32, TextureHint::LINEAR } }, false, context));
    // TODO: Should tex map buffer be linearly sampled or nearest? (Currently linear)
    //     Linear would make smoother lightmaps
    //     Nearest would prevent bleeding between geometry edges
    probe_map_buffer_.reset(new Framebuffer(kProbeMapSize * 6, kProbeMapSize * static_cast<int>(probes_.size()), 2, context));
    probe_buffer_.reset(new Framebuffer(kProbeMapSize * 6, kProbeMapSize * static_cast<int>(probes_.size()), { { TextureHint::R8G8B8, TextureHint::NEAREST } }, context));
    probe_coefficients_buffer_.reset(new Framebuffer(9, static_cast<int>(probes_.size()), { { TextureHint::R8G8B8, TextureHint::NEAREST } }, context));
}

// Renders the scene geometry to a lightmap by using the light-depth buffer
// with the lightmap UVs as vertex coordinates
bool Lightprobe::Render(const Scene& scene, const Geometry& geometry, const Shadow& shadow, Perspective perspective, Matrix view_matrix, Matrix proj_matrix, Matrix light_vp_matrix, RenderContext& context)
{
    if (!RenderLightmaps(scene, shadow, light_vp_matrix, context))
    {
        return false;
    }

    // Used to turn pixel fragments into world coordinates
    Matrix inv_proj_view = MatrixInverse(view_matrix * proj_matrix);

    // Needed so sprites can render over themselves
    context->SetDepthTesting(false);

    // Add the sum of all overlapping probes per pixel
    context->SetBlendMode(ADDITIVE);
    // Only render back faces... This is to prevent probes getting clipped
    // as you enter their radius
    context->SetCullMode(ENABLE_CW);

    indirect_light_buffer_->Bind(context);

    // Render all of the probes as spheres to find the pixels where they overlap
    // with scene geometry. This is much faster than sampling 50 probes for every pixel
    probe_meshes_->Render(false, context);

    // Do the indirect lighting from the SH coefficients
    if (!indirect_light_shader_->SetInput("mvp_matrix", view_matrix * proj_matrix, context) ||
        !indirect_light_shader_->SetInput("inv_vp_matrix", inv_proj_view, context) ||
        !indirect_light_shader_->SetInput("screen", Vector2(units::pixel_to_subpixel(perspective.width),
                                                            units::pixel_to_subpixel(perspective.height)), context) ||
        !indirect_light_shader_->SetInput("probe_count", static_cast<float>(probes_.size()), context) ||
        !indirect_light_shader_->SetInput("probe_distance", kProbeDistance, context) ||
        !indirect_light_shader_->SetInput("normal", geometry.output(stage::Geometry::NORMAL), 0, context) ||
        !indirect_light_shader_->SetInput("view_depth", geometry.output(stage::Geometry::DEPTH), 1, context) ||
        !indirect_light_shader_->SetInput("probe_coefficients", probe_coefficients_buffer_->textures()[0], 2, context))
    {
        return false;
    }

    // Finally do the render
    if (!indirect_light_shader_->Render(probe_meshes_->index_count(), context))
    {
        return false;
    }
    context->SetBlendMode(ALPHA);
    context->SetCullMode(ENABLE_CCW);

    return true;
}

bool Lightprobe::BuildLighting(const Scene& scene, RenderContext& context)
{
    context->BeginScene(Vector4(0, 0, 0, 1));
    // Mark the position and normal of every vertex on the lightmap
    if (!BuildLightMapLookups(scene, context))
    {
        return false;
    }
    // Render out a cube map for every probe to sample scene data from (should only be called once per map load... put in make context?)
    if (!BuildProbeMaps(scene, context))
    {
        return false;
    }
    return true;
}

const TextureResource* Lightprobe::output(Output buffer) const
{
    switch (buffer)
    {
    case COEFFICIENTS:
        return probe_coefficients_buffer_->textures()[0];
        break;
    case LIGHT_MAP_LOOKUP_POS:
        return light_map_lookup_buffer_->textures()[0];
        break;
    case LIGHT_MAP_LOOKUP_NORMAL:
        return light_map_lookup_buffer_->textures()[1];
        break;
    case DIRECT_LIGHT_MAP:
        return direct_light_map_accumulation_buffer_->textures()[0];
        break;
    case INDIRECT_LIGHT_MAP:
        return indirect_light_map_accumulation_buffer_->textures()[0];
        break;
    case INDIRECT_LIGHT:
        return indirect_light_buffer_->textures()[0];
        break;
    case PROBE_ALBEDO:
        return probe_map_buffer_->textures()[0];
        break;
    case PROBE_UV:
        return probe_map_buffer_->textures()[1];
        break;
    case PROBE:
        return probe_buffer_->textures()[0];
        break;
    default:
        return nullptr;
    }
}

void Lightprobe::GenerateProbes()
{
    // Initialize our light probes...
    for (int x = -15; x <= 15; x += 5)
    {
        for (int y = 3; y <= 8; y += 5)
        {
            for (int z = -5; z <= 5; z += 5)
            {
                probes_.push_back(Vector3(static_cast<units::world>(x),
                    static_cast<units::world>(y),
                    static_cast<units::world>(z)));
            }
        }
    }
    for (int x = -8; x <= 8; x += 4)
    {
        int y = 12;
        int z = 0;
        probes_.push_back(Vector3(static_cast<units::world>(x),
            static_cast<units::world>(y),
            static_cast<units::world>(z)));
    }
}

// Writes out the position and normal of every model vertex to
// the lightmap. This makes it so we don't need to re-render scene
// geometry when we do lightmap passes
bool Lightprobe::BuildLightMapLookups(const Scene& scene, RenderContext& context)
{
    context->SetDepthTesting(false);
    // Since lightmap UVs have no regard for vertex winding
    context->SetCullMode(DISABLE);

    light_map_lookup_buffer_->Bind(Vector4(0, 0, 0, 1), context);
    for (const auto& model : scene.models)
    {
        // Bind the vertex data
        model->Render(context);

        // Make the draw call
        if (!light_map_lookup_shader_->Render(model->index_count(), context))
        {
            return false;
        }
    }
    context->SetCullMode(ENABLE_CCW);

    return true;
}

bool Lightprobe::BuildProbeMaps(const Scene& scene, RenderContext& context)
{
    context->SetDepthTesting(false);
    // Since the UV pass only writes to 2 channels, we want the clear colour
    // to be "all on" in the third channel so we can tell when there is no
    // valid value to sample (looking at the sky)
    // However the diffuse pass should use the sky_colour_ when looking at the
    // sky... so we solve this by doing a clear-pass over the textures where we
    // write the proper clear colours to each one (like a unique pseudo-skybox)
    probe_map_buffer_->Bind(context);

    probe_map_buffer_->Render(context);
    if (!probe_map_clear_shader_->SetInput("proj_matrix", probe_ortho_matrix_, context) ||
        !probe_map_clear_shader_->SetInput("clear_colour", Vector3(0.0, 0.0, 1.0), context) ||
        !probe_map_clear_shader_->SetInput("sky_colour", scene.sky_colour, context))
    {
        return false;
    }
    if (!probe_map_clear_shader_->Render(probe_map_buffer_->index_count(), context))
    {
        return false;
    }

    // And now we actually render the scene into the probe maps...
    context->SetDepthTesting(true);
    auto render_models = [&](const Matrix& view)
    {
        // TODO: This can be batched for sure
        // TODO: Make sure only static objects are rendered
        for (const auto& model : scene.models)
        {
            Matrix view_proj = view * probe_proj_matrix_;

            // Bind the vertex data
            model->Render(context);

            Matrix model_view_proj = model->world_matrix() * view_proj;
            // Set the inputs
            if (!probe_map_shader_->SetInput("mvp_matrix", model_view_proj, context) ||
                !probe_map_shader_->SetInput("albedo", model->albedo(), 0, context))
            {
                return false;
            }

            // Make the draw call
            if (!probe_map_shader_->Render(model->index_count(), context))
            {
                return false;
            }
        }
        return true;
    };

    // Render a 2 cubemaps per probe
    //     One containing diffuse colour
    //     One containing lightmap coords to use for sampling from/writing to lightmap
    for (int i = 0; i < probes_.size(); i++)
    {
        probe_view_->set_pos(probes_[i].x, probes_[i].y, probes_[i].z);
        bool success = true;
        // Front view
        context->SetViewport(Box(kProbeMapSize * 0, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(0.0f, 0.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Rear view
        context->SetViewport(Box(kProbeMapSize * 1, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(0.0f, kPi, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Right view
        context->SetViewport(Box(kProbeMapSize * 2, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(0.0f, kPi / -2.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Left view
        context->SetViewport(Box(kProbeMapSize * 3, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(0.0f, kPi / 2.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Up view
        context->SetViewport(Box(kProbeMapSize * 4, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(kPi / 2.0f, 0.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Down view
        context->SetViewport(Box(kProbeMapSize * 5, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(kPi / -2.0f, 0.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        if (!success)
        {
            return false;
        }
    }
    probe_map_buffer_->Unbind(context);

    probe_meshes_.reset(new DrawBatcher(DrawBatcher::MESH_3D, context));
    for (int i = 0; i < probes_.size(); i++)
    {
        std::unique_ptr<Mesh> probe_sphere(new Mesh("blons:sphere~" + std::to_string(kProbeDistance), context));
        MeshData probe_mesh = probe_sphere->mesh();
        // Hacky way to let us batch the probe sphere call:
        // Instead of using a uniform with the probe's central position & a separate draw call
        // for each probe, we can batch them all and inject the probe position into the
        // vertex's normal data
        for (auto& v : probe_mesh.vertices)
        {
            v.tex.x = static_cast<float>(i);
            v.norm = Vector3(probes_[i].x, probes_[i].y, probes_[i].z);
        }
        probe_meshes_->Append(probe_mesh, MatrixTranslation(probes_[i].x, probes_[i].y, probes_[i].z), context);
    }

    probe_quads_.reset(new DrawBatcher(DrawBatcher::MESH_3D, context));
    for (int i = 0; i < probes_.size(); i++)
    {
        std::unique_ptr<Mesh> probe_quad(new Mesh("blons:quad~" + std::to_string(kLightMapResolution), context));
        MeshData probe_mesh = probe_quad->mesh();
        for (auto& v : probe_mesh.vertices)
        {
            v.tex.x = static_cast<float>(i);
            v.norm = Vector3(probes_[i].x, probes_[i].y, probes_[i].z);
        }
        probe_quads_->Append(probe_mesh, MatrixTranslation(probes_[i].x, probes_[i].y, probes_[i].z), context);
    }
    return true;
}

bool Lightprobe::RenderLightmaps(const Scene& scene, const Shadow& shadow, Matrix light_vp_matrix, RenderContext& context)
{
    // Can be removed when we support more lights
    assert(scene.lights.size() == 1);
    Light* sun = scene.lights[0];

    // Rendering to a 2D map so whatever!!!
    context->SetDepthTesting(false);
    // Incase lightmap UVs are imperfect, allow them to overwrite themselves
    context->SetBlendMode(ADDITIVE);
    // Since lightmap UVs have no regard for vertex winding
    context->SetCullMode(DISABLE);

    direct_light_map_accumulation_buffer_->Bind(Vector4(0, 0, 0, 1), context);
    // Bind the vertex data
    direct_light_map_accumulation_buffer_->Render(context);

    // Set the inputs
    if (!direct_light_map_shader_->SetInput("proj_matrix", light_map_ortho_matrix_, context) ||
        !direct_light_map_shader_->SetInput("light_vp_matrix", light_vp_matrix, context) ||
        !direct_light_map_shader_->SetInput("pos_lookup", light_map_lookup_buffer_->textures()[0], 0, context) ||
        !direct_light_map_shader_->SetInput("norm_lookup", light_map_lookup_buffer_->textures()[1], 1, context) ||
        !direct_light_map_shader_->SetInput("light_depth", shadow.output(stage::Shadow::LIGHT_DEPTH), 2, context) ||
        !direct_light_map_shader_->SetInput("sun.colour", sun->colour(), context) ||
        !direct_light_map_shader_->SetInput("sun.dir", sun->direction(), context))
    {
        return false;
    }

    // Make the draw call
    if (!direct_light_map_shader_->Render(direct_light_map_accumulation_buffer_->index_count(), context))
    {
        return false;
    }
    context->SetBlendMode(ALPHA);
    context->SetCullMode(ENABLE_CCW);

    // Clear the bounce lightmap before calling probe shader
    indirect_light_map_accumulation_buffer_->Bind(Vector4(0, 0, 0, 0), context);

    // Build up the SH coefficients
    SumCoefficients(scene, context);

    // This is pretty expensive, 1 bounce adds a lot and works great
    for (int i = 0; i < kLightBounces; i++)
    {
        context->SetDepthTesting(false);
        // Stores multiple bounces
        context->SetBlendMode(ADDITIVE);
        indirect_light_map_accumulation_buffer_->Bind(false, context);

        // Render each probe as a quad covering the entire lightmap to have it apply globally
        probe_quads_->Render(false, context);

        // Set the inputs
        if (!indirect_light_map_shader_->SetInput("proj_matrix", light_map_ortho_matrix_, context) ||
            !indirect_light_map_shader_->SetInput("lightmap_resolution", static_cast<float>(kLightMapResolution), context) ||
            !indirect_light_map_shader_->SetInput("probe_count", static_cast<float>(probes_.size()), context) ||
            !indirect_light_map_shader_->SetInput("probe_distance", kProbeDistance, context) ||
            !indirect_light_map_shader_->SetInput("pos_lookup", light_map_lookup_buffer_->textures()[0], 0, context) ||
            !indirect_light_map_shader_->SetInput("norm_lookup", light_map_lookup_buffer_->textures()[1], 1, context) ||
            !indirect_light_map_shader_->SetInput("probe_coefficients", probe_coefficients_buffer_->textures()[0], 2, context))
        {
            return false;
        }

        // Make the draw call
        if (!indirect_light_map_shader_->Render(probe_quads_->index_count(), context))
        {
            return false;
        }
        context->SetBlendMode(ALPHA);

        // Build up the SH coefficients
        SumCoefficients(scene, context);
    }
    return true;
}

bool Lightprobe::SumCoefficients(const Scene& scene, RenderContext& context)
{
    // Render lightmap to our light probes
    probe_buffer_->Bind(context);
    probe_buffer_->Render(context);
    // Set the inputs
    if (!probe_shader_->SetInput("proj_matrix", probe_ortho_matrix_, context) ||
        !probe_shader_->SetInput("probe_albedo", probe_map_buffer_->textures()[0], 0, context) ||
        !probe_shader_->SetInput("probe_texmap", probe_map_buffer_->textures()[1], 1, context) ||
        !probe_shader_->SetInput("direct_lightmap", direct_light_map_accumulation_buffer_->textures()[0], 2, context) ||
        !probe_shader_->SetInput("indirect_lightmap", indirect_light_map_accumulation_buffer_->textures()[0], 3, context) ||
        !probe_shader_->SetInput("sky_colour", scene.sky_colour, context))
    {
        return false;
    }

    // Make the draw call
    if (!probe_shader_->Render(probe_buffer_->index_count(), context))
    {
        return false;
    }

    // Render lightmap to our light probes
    probe_coefficients_buffer_->Bind(context);
    probe_coefficients_buffer_->Render(context);

    Matrix coef_ortho_matrix = MatrixOrthographic(0,
                                                  static_cast<units::world>(9),
                                                  static_cast<units::world>(probes_.size()),
                                                  0, 0, 1);
    // Set the inputs
    if (!probe_coefficients_shader_->SetInput("proj_matrix", coef_ortho_matrix, context) ||
        !probe_coefficients_shader_->SetInput("probe_irradiance", probe_buffer_->textures()[0], 0, context) ||
        !probe_coefficients_shader_->SetInput("probe_count", static_cast<int>(probes_.size()), context) ||
        !probe_coefficients_shader_->SetInput("probe_map_size", kProbeMapSize, context))
    {
        return false;
    }
    // Make the draw call
    if (!probe_coefficients_shader_->Render(probe_coefficients_buffer_->index_count(), context))
    {
        return false;
    }
    return true;
}
} // namespace stage
} // namespace pipeline
} // namespace blons
