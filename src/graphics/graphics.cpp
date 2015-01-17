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

#include <blons/graphics/graphics.h>

// Public Includes
#include <blons/graphics/gui/gui.h>
#include <blons/graphics/light.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>
#include <blons/math/math.h>
#include <blons/temphelpers.h>
// Local Includes
#include "render/renderd3d11.h"
#include "render/rendergl40.h"
#include "resource.h"

namespace blons
{
// Managed assets that allows the blons::Graphics class to track and render anything it creates
class ManagedModel : public Model
{
public:
    ManagedModel(std::string filename, RenderContext& context) : Model(filename, context) {}
    ~ManagedModel() override;
private:
    friend Graphics;
    void Finish();
    std::function<void(ManagedModel*)> deleter_;
};

class ManagedSprite : public Sprite
{
public:
    ManagedSprite(std::string filename, RenderContext& context) : Sprite(filename, context) {}
    ~ManagedSprite() override;
private:
    friend Graphics;
    void Finish();
    std::function<void(ManagedSprite*)> deleter_;
};

Graphics::Graphics(Client::Info screen)
{
    if (!MakeContext(screen))
    {
        log::Fatal("Failed to initialize rendering context\n");
        throw "Failed to initiralize rendering context";
    }

    // Camera
    camera_.reset(new Camera);
    if (camera_ == nullptr)
    {
        throw "Failed to initialize camera";
    }

    camera_->set_pos(0.0f, 2.0f, 0.0f);

    // Probe camera
    probe_view_.reset(new Camera);
    if (probe_view_ == nullptr)
    {
        throw "Failed to initialize probe camera";
    }

    // Sunlight
    // TODO: User should be doing this somehow
    sun_.reset(new Light(Light::DIRECTIONAL,
                         Vector3(20.0f, 20.0f, 5.0f), // position
                         // Midnight
                         //Vector3(-10.0f, 0.0f, -5.0f), // direction
                         //Vector3(0.1f, 0.05f, 0.03f))); // colour
                         // Sunset
                         //Vector3(-10.0f, -2.0f, -5.0f), // direction
                         //Vector3(1.0f, 0.75, 0.4f))); // colour
                         // Noon
                         Vector3(-2.0f, -5.0f, -0.5f),
                         Vector3(1.0f, 0.95f, 0.8f))); // colour
    // Sunny day
    sky_colour_ = Vector3(0.3f, 0.6f, 1.0f);
    // Midnight
    //sky_colour_ = Vector3(0.1f, 0.1f, 0.4f);
}

Graphics::~Graphics()
{
    // If graphics gets deleted before its models & sprites, make sure
    // they're cleaned up safely
    while (models_.size() > 0)
    {
        auto m = *models_.begin();
        m->Finish();
        // Just to be safe
        models_.erase(m);
    }
    while (sprites_.size() > 0)
    {
        auto s = *sprites_.begin();
        s->Finish();
        // Just to be safe
        sprites_.erase(s);
    }
}

std::unique_ptr<Model> Graphics::MakeModel(std::string filename)
{
    auto model = new ManagedModel(filename, context_);
    model->deleter_ = [&](ManagedModel* m)
    {
        models_.erase(m);
    };
    models_.insert(model);
    return std::unique_ptr<Model>(model);
}

std::unique_ptr<Sprite> Graphics::MakeSprite(std::string filename)
{
    auto sprite = new ManagedSprite(filename, context_);
    sprite->deleter_ = [&](ManagedSprite* s)
    {
        sprites_.erase(s);
    };
    sprites_.insert(sprite);
    return std::unique_ptr<Sprite>(sprite);
}

bool Graphics::Render()
{
    // Calculates view_matrix from scratch, so we cache it
    Matrix view_matrix = camera_->view_matrix();
    Matrix light_vp_matrix = sun_->ViewFrustum(view_matrix * proj_matrix_, kScreenDepth);

    // Clear buffers
    context_->BeginScene(Vector4(0, 0, 0, 1));

    // Render all of the geometry and accompanying info (normal, depth, etc)
    if (!RenderGeometry(view_matrix))
    {
        return false;
    }

    // Render all of the geometry and get their depth from the light
    // Then render a shadow map from the depth information
    if (!RenderShadowMaps(view_matrix, light_vp_matrix))
    {
        return false;
    }

    // Render all of the geometry a few more times... :(
    // Builds a direct light map and then bounce lighting
    // Need to render geometry to get fragment position right
    if (!RenderLightMaps(light_vp_matrix))
    {
        return false;
    }

    if (!RenderLighting(view_matrix))
    {
        return false;
    }

    // Bind the back buffer
    context_->BindFramebuffer(nullptr);

    // Render the final composite of the geometry and lighting passes
    if (!RenderComposite())
    {
        return false;
    }

    // Render all 2D sprites
    if (!RenderSprites())
    {
        return false;
    }

    // Render the GUI
    gui_->Render(context_);

    // Swap buffers
    context_->EndScene();

    return true;
}

// Writes out the position and normal of every model vertex to
// the lightmap. This makes it so we don't need to re-render scene
// geometry when we do lightmap passes
bool Graphics::BuildLightMapLookups()
{
    context_->SetDepthTesting(false);
    // Since lightmap UVs have no regard for vertex winding
    context_->SetCullMode(DISABLE);

    light_map_lookup_buffer_->Bind(Vector4(0, 0, 0, 1), context_);
    for (const auto& model : models_)
    {
        // Bind the vertex data
        model->Render(context_);

        // Make the draw call
        if (!light_map_lookup_shader_->Render(model->index_count(), context_))
        {
            return false;
        }
    }
    context_->SetCullMode(ENABLE_CCW);

    return true;
}

bool Graphics::BuildProbeMaps()
{
    context_->SetDepthTesting(false);
    // Since the UV pass only writes to 2 channels, we want the clear colour
    // to be "all on" in the third channel so we can tell when there is no
    // valid value to sample (looking at the sky)
    // However the diffuse pass should use the sky_colour_ when looking at the
    // sky... so we solve this by doing a clear-pass over the textures where we
    // write the proper clear colours to each one (like a unique pseudo-skybox)
    probe_map_buffer_->Bind(context_);

    probe_map_buffer_->Render(context_);
    if (!probe_map_clear_shader_->SetInput("proj_matrix", probe_ortho_matrix_, context_) ||
        !probe_map_clear_shader_->SetInput("clear_colour", Vector3(0.0, 0.0, 1.0), context_) ||
        !probe_map_clear_shader_->SetInput("sky_colour", sky_colour_, context_))
    {
        return false;
    }
    if (!probe_map_clear_shader_->Render(probe_map_buffer_->index_count(), context_))
    {
        return false;
    }

    // And now we actually render the scene into the probe maps...
    context_->SetDepthTesting(true);
    auto render_models = [&](const Matrix& view)
    {
        // TODO: This can be batch for sure
        // TODO: Make sure only static objects are rendered
        for (const auto& model : models_)
        {
            Matrix view_proj = view * probe_proj_matrix_;

            // Bind the vertex data
            model->Render(context_);

            Matrix model_view_proj = model->world_matrix() * view_proj;
            // Set the inputs
            if (!probe_map_shader_->SetInput("mvp_matrix", model_view_proj, context_) ||
                !probe_map_shader_->SetInput("albedo", model->albedo(), 0, context_))
            {
                return false;
            }

            // Make the draw call
            if (!probe_map_shader_->Render(model->index_count(), context_))
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
        context_->SetViewport(Box(kProbeMapSize * 0, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(0.0f, 0.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Rear view
        context_->SetViewport(Box(kProbeMapSize * 1, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(0.0f, kPi, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Right view
        context_->SetViewport(Box(kProbeMapSize * 2, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(0.0f, kPi / -2.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Left view
        context_->SetViewport(Box(kProbeMapSize * 3, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(0.0f, kPi / 2.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Up view
        context_->SetViewport(Box(kProbeMapSize * 4, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(kPi / 2.0f, 0.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        // Down view
        context_->SetViewport(Box(kProbeMapSize * 5, kProbeMapSize * i, kProbeMapSize, kProbeMapSize));
        probe_view_->set_rot(kPi / -2.0f, 0.0f, 0.0f);
        success &= render_models(probe_view_->view_matrix());

        if (!success)
        {
            return false;
        }
    }
    probe_map_buffer_->Unbind(context_);

    probe_meshes_.reset(new DrawBatcher(DrawBatcher::MESH_3D, context_));
    for (int i = 0; i < probes_.size(); i++)
    {
        std::unique_ptr<Mesh> probe_sphere(new Mesh("blons:sphere~" + std::to_string(kProbeDistance), context_));
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
        probe_meshes_->Append(probe_mesh, MatrixTranslation(probes_[i].x, probes_[i].y, probes_[i].z), context_);
    }

    probe_quads_.reset(new DrawBatcher(DrawBatcher::MESH_3D, context_));
    for (int i = 0; i < probes_.size(); i++)
    {
        std::unique_ptr<Mesh> probe_quad(new Mesh("blons:quad~" + std::to_string(kLightMapResolution), context_));
        MeshData probe_mesh = probe_quad->mesh();
        for (auto& v : probe_mesh.vertices)
        {
            v.tex.x = static_cast<float>(i);
            v.norm = Vector3(probes_[i].x, probes_[i].y, probes_[i].z);
        }
        probe_quads_->Append(probe_mesh, MatrixTranslation(probes_[i].x, probes_[i].y, probes_[i].z), context_);
    }
    return true;
}

bool Graphics::RenderGeometry(Matrix view_matrix)
{
    // Needed so models dont render over themselves
    context_->SetDepthTesting(true);

    // Bind the geometry framebuffer to render all models onto
    geometry_buffer_->Bind(context_);

    Matrix view_proj = view_matrix * proj_matrix_;
    // TODO: 3D pass ->
    //      Render static world geo as batches without world matrix
    //      Render movable objects singularly with world matrix
    for (const auto& model : models_)
    {
        // Bind the vertex data
        model->Render(context_);

        Matrix model_view_proj = model->world_matrix() * view_proj;
        // Set the inputs
        if (!geo_shader_->SetInput("mvp_matrix", model_view_proj, context_) ||
            !geo_shader_->SetInput("albedo", model->albedo(), 0, context_) ||
            !geo_shader_->SetInput("normal", model->normal(), 1, context_))
        {
            return false;
        }

        // Make the draw call
        if (!geo_shader_->Render(model->index_count(), context_))
        {
            return false;
        }
    }
    return true;
}

bool Graphics::RenderShadowMaps(Matrix view_matrix, Matrix light_vp_matrix)
{
    // TODO: Parallel split shadow maps
    //     Shouldn't be much harder than splitting clip distance in ndc_box of sun_->ViewFrustum
    //     along a linear blend of logarithmic and uniform splits
    // Bind the shadow depth framebuffer to render all models onto
    shadow_buffer_->Bind(context_);

    // TODO: 3D pass ->
    //      Render everything as a batch as this is untextured
    for (const auto& model : models_)
    {
        // Bind the vertex data
        model->Render(context_);

        Matrix mvp_matrix = model->world_matrix() * light_vp_matrix;
        // Set the inputs
        if (!shadow_shader_->SetInput("mvp_matrix", mvp_matrix, context_))
        {
            return false;
        }

        // Make the draw call
        if (!shadow_shader_->Render(model->index_count(), context_))
        {
            return false;
        }
    }

    // Bit of an awkward hack to save VRAM:
    //     Bind blur buffer, use shadow depth as input and write a horizontal blur to blur buffer out
    //     Bind shadow buffer, use horizontal blur buffer as input and write a vertical blur to shadow buffer out
    // Result is an efficient O(2n) box blur (instead of O(n^2)!) needing only 2 textures (instead of 3)

    // Blur the shadow map to make soft shadows
    blur_buffer_->Bind(context_);

    // Using geometry buffer's quad mesh because it matches our ortho matrix better
    geometry_buffer_->Render(context_);

    // Horizontal blur
    if (!blur_shader_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !blur_shader_->SetInput("blur_texture", shadow_buffer_->textures()[0], context_) ||
        !blur_shader_->SetInput("texture_resolution", kShadowMapResolution, context_) ||
        !blur_shader_->SetInput("direction", 0, context_))
    {
        return false;
    }
    if (!blur_shader_->Render(geometry_buffer_->index_count(), context_))
    {
        return false;
    }

    // Blur the shadow map to make soft shadows
    shadow_buffer_->Bind(context_);

    // Using geometry buffer's quad mesh because it matches our ortho matrix better
    geometry_buffer_->Render(context_);

    // Veritcal blur
    if (!blur_shader_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !blur_shader_->SetInput("blur_texture", blur_buffer_->textures()[0], context_) ||
        !blur_shader_->SetInput("texture_resolution", kShadowMapResolution, context_) ||
        !blur_shader_->SetInput("direction", 1, context_))
    {
        return false;
    }
    if (!blur_shader_->Render(geometry_buffer_->index_count(), context_))
    {
        return false;
    }

    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);

    // Bind the shadow depth framebuffer to render all models onto
    direct_light_buffer_->Bind(context_);

    // Render the geometry as a sprite
    geometry_buffer_->Render(context_);

    // Used to turn pixel fragments into world coordinates
    Matrix inv_proj_view = MatrixInverse(view_matrix * proj_matrix_);

    // Set the inputs
    if (!direct_light_shader_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !direct_light_shader_->SetInput("inv_vp_matrix", inv_proj_view, context_) ||
        !direct_light_shader_->SetInput("light_vp_matrix", light_vp_matrix, context_) ||
        !direct_light_shader_->SetInput("view_depth", geometry_buffer_->depth(), 0, context_) ||
        !direct_light_shader_->SetInput("light_depth", shadow_buffer_->textures()[0], 1, context_) ||
        !direct_light_shader_->SetInput("normal", geometry_buffer_->textures()[1], 2, context_) ||
        !direct_light_shader_->SetInput("sun.dir", sun_->direction(), context_))
    {
        return false;
    }

    // Finally do the render
    if (!direct_light_shader_->Render(geometry_buffer_->index_count(), context_))
    {
        return false;
    }
    return true;
}

// Renders the scene geometry to a lightmap by using the light-depth buffer
// with the lightmap UVs as vertex coordinates
bool Graphics::RenderLightMaps(Matrix light_vp_matrix)
{
    // Rendering to a 2D map so whatever!!!
    context_->SetDepthTesting(false);
    // Incase lightmap UVs are imperfect, allow them to overwrite themselves
    context_->SetBlendMode(ADDITIVE);
    // Since lightmap UVs have no regard for vertex winding
    context_->SetCullMode(DISABLE);

    direct_light_map_accumulation_buffer_->Bind(Vector4(0, 0, 0, 1), context_);
    // Bind the vertex data
    direct_light_map_accumulation_buffer_->Render(context_);

    // Set the inputs
    if (!direct_light_map_shader_->SetInput("proj_matrix", light_map_ortho_matrix_, context_) ||
        !direct_light_map_shader_->SetInput("light_vp_matrix", light_vp_matrix, context_) ||
        !direct_light_map_shader_->SetInput("pos_lookup", light_map_lookup_buffer_->textures()[0], 0, context_) ||
        !direct_light_map_shader_->SetInput("norm_lookup", light_map_lookup_buffer_->textures()[1], 1, context_) ||
        !direct_light_map_shader_->SetInput("light_depth", shadow_buffer_->textures()[0], 2, context_) ||
        !direct_light_map_shader_->SetInput("sun.colour", sun_->colour(), context_) ||
        !direct_light_map_shader_->SetInput("sun.dir", sun_->direction(), context_))
    {
        return false;
    }

    // Make the draw call
    if (!direct_light_map_shader_->Render(direct_light_map_accumulation_buffer_->index_count(), context_))
    {
        return false;
    }
    context_->SetBlendMode(ALPHA);
    context_->SetCullMode(ENABLE_CCW);

    // Clear the bounce lightmap before calling probe shader
    indirect_light_map_accumulation_buffer_->Bind(Vector4(0, 0, 0, 0), context_);

    // Build up the SH coefficients
    SumCoefficients();

    // This is pretty expensive, 1 bounce adds a lot and works great
    for (int i = 0; i < kLightBounces; i++)
    {
        context_->SetDepthTesting(false);
        // Stores multiple bounces
        context_->SetBlendMode(ADDITIVE);
        indirect_light_map_accumulation_buffer_->Bind(false, context_);

        // Render each probe as a quad covering the entire lightmap to have it apply globally
        probe_quads_->Render(false, context_);

        // Set the inputs
        if (!indirect_light_map_shader_->SetInput("proj_matrix", light_map_ortho_matrix_, context_) ||
            !indirect_light_map_shader_->SetInput("lightmap_resolution", static_cast<float>(kLightMapResolution), context_) ||
            !indirect_light_map_shader_->SetInput("probe_count", static_cast<float>(probes_.size()), context_) ||
            !indirect_light_map_shader_->SetInput("probe_distance", kProbeDistance, context_) ||
            !indirect_light_map_shader_->SetInput("pos_lookup", light_map_lookup_buffer_->textures()[0], 0, context_) ||
            !indirect_light_map_shader_->SetInput("norm_lookup", light_map_lookup_buffer_->textures()[1], 1, context_) ||
            !indirect_light_map_shader_->SetInput("probe_coefficients", probe_coefficients_buffer_->textures()[0], 2, context_))
        {
            return false;
        }

        // Make the draw call
        if (!indirect_light_map_shader_->Render(probe_quads_->index_count(), context_))
        {
            return false;
        }
        context_->SetBlendMode(ALPHA);

        // Build up the SH coefficients
        SumCoefficients();
    }
    return true;
}

bool Graphics::SumCoefficients()
{
    // Render lightmap to our light probes
    probe_buffer_->Bind(context_);
    probe_buffer_->Render(context_);
    // Set the inputs
    if (!probe_shader_->SetInput("proj_matrix", probe_ortho_matrix_, context_) ||
        !probe_shader_->SetInput("probe_albedo", probe_map_buffer_->textures()[0], 0, context_) ||
        !probe_shader_->SetInput("probe_texmap", probe_map_buffer_->textures()[1], 1, context_) ||
        !probe_shader_->SetInput("direct_lightmap", direct_light_map_accumulation_buffer_->textures()[0], 2, context_) ||
        !probe_shader_->SetInput("indirect_lightmap", indirect_light_map_accumulation_buffer_->textures()[0], 3, context_) ||
        !probe_shader_->SetInput("sky_colour", sky_colour_, context_))
    {
        return false;
    }

    // Make the draw call
    if (!probe_shader_->Render(probe_buffer_->index_count(), context_))
    {
        return false;
    }

    // Render lightmap to our light probes
    probe_coefficients_buffer_->Bind(context_);
    probe_coefficients_buffer_->Render(context_);

    Matrix coef_ortho_matrix = MatrixOrthographic(0,
                                                  static_cast<units::world>(9),
                                                  static_cast<units::world>(probes_.size()),
                                                  0, 0, 1);
    // Set the inputs
    if (!probe_coefficients_shader_->SetInput("proj_matrix", coef_ortho_matrix, context_) ||
        !probe_coefficients_shader_->SetInput("probe_irradiance", probe_buffer_->textures()[0], 0, context_) ||
        !probe_coefficients_shader_->SetInput("probe_count", static_cast<int>(probes_.size()), context_) ||
        !probe_coefficients_shader_->SetInput("probe_map_size", kProbeMapSize, context_))
    {
        return false;
    }
    // Make the draw call
    if (!probe_coefficients_shader_->Render(probe_coefficients_buffer_->index_count(), context_))
    {
        return false;
    }
    return true;
}

bool Graphics::RenderLighting(Matrix view_matrix)
{
    // Used to turn pixel fragments into world coordinates
    Matrix inv_proj_view = MatrixInverse(view_matrix * proj_matrix_);

    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);

    // Add the sum of all overlapping probes per pixel
    context_->SetBlendMode(ADDITIVE);
    // Only render back faces... This is to prevent probes getting clipped
    // as you enter their radius
    context_->SetCullMode(ENABLE_CW);

    indirect_light_buffer_->Bind(context_);

    // Render all of the probes as spheres to find the pixels where they overlap
    // with scene geometry. This is much faster than sampling 50 probes for every pixel
    probe_meshes_->Render(false, context_);

    // Do the indirect lighting from the SH coefficients
    if (!indirect_light_shader_->SetInput("mvp_matrix", view_matrix * proj_matrix_, context_) ||
        !indirect_light_shader_->SetInput("inv_vp_matrix", inv_proj_view, context_) ||
        !indirect_light_shader_->SetInput("screen", Vector2(units::pixel_to_subpixel(screen_.width),
                                                            units::pixel_to_subpixel(screen_.height)), context_) ||
        !indirect_light_shader_->SetInput("probe_count", static_cast<float>(probes_.size()), context_) ||
        !indirect_light_shader_->SetInput("probe_distance", kProbeDistance, context_) ||
        !indirect_light_shader_->SetInput("normal", geometry_buffer_->textures()[1], 0, context_) ||
        !indirect_light_shader_->SetInput("view_depth", geometry_buffer_->depth(), 1, context_) ||
        !indirect_light_shader_->SetInput("probe_coefficients", probe_coefficients_buffer_->textures()[0], 2, context_))
    {
        return false;
    }

    // Finally do the render
    if (!indirect_light_shader_->Render(probe_meshes_->index_count(), context_))
    {
        return false;
    }
    context_->SetBlendMode(ALPHA);
    context_->SetCullMode(ENABLE_CCW);

    // Bind the buffer to do all lighting calculations on
    light_buffer_->Bind(context_);

    // Render the geometry as a sprite
    geometry_buffer_->Render(context_);

    // Set the inputs
    if (!light_shader_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !light_shader_->SetInput("inv_vp_matrix", inv_proj_view, context_) ||
        !light_shader_->SetInput("albedo", geometry_buffer_->textures()[0], 0, context_) ||
        !light_shader_->SetInput("normal", geometry_buffer_->textures()[1], 1, context_) ||
        !light_shader_->SetInput("depth", geometry_buffer_->depth(), 2, context_) ||
        !light_shader_->SetInput("direct_light", direct_light_buffer_->textures()[0], 3, context_) ||
        !light_shader_->SetInput("indirect_light", indirect_light_buffer_->textures()[0], 4, context_) ||
        !light_shader_->SetInput("sun.dir", sun_->direction(), context_) ||
        !light_shader_->SetInput("sun.colour", sun_->colour(), context_) ||
        !light_shader_->SetInput("sky_colour", sky_colour_, context_))
    {
        return false;
    }

    // Finally do the render
    if (!light_shader_->Render(geometry_buffer_->index_count(), context_))
    {
        return false;
    }
    return true;
}

bool Graphics::RenderComposite()
{
    // TODO: TEMP DEBUG CODE. CLEAN THIS UP!!!
    static bool init = true;
    static const console::Variable* target = nullptr;
    static std::unique_ptr<Sprite> target_sprite(new Sprite("blons:none", context_));
    static std::unique_ptr<Sprite> alt_target(new Sprite("blons:none", context_));
    if (init)
    {
        console::RegisterVariable("gfx:target", 0);
        target = console::var("gfx:target");
        init = false;
        alt_target->set_pos(1380, 660, 200, 200);
        alt_target->set_subtexture(0, 0, 16, -16);
    }
    target_sprite->set_pos(0, 0, screen_.width, screen_.height);
    target_sprite->set_subtexture(0, 0, 16, -16);
    const TextureResource* screen_texture;
    const TextureResource* alt_screen_texture;
    switch (target->to<int>())
    {
    case 1:
        screen_texture = geometry_buffer_->textures()[0];
        break;
    case 2:
        screen_texture = geometry_buffer_->textures()[1];
        break;
    case 3:
        screen_texture = geometry_buffer_->textures()[2];
        break;
    case 4:
        screen_texture = geometry_buffer_->depth();
        break;
    case 5:
        screen_texture = shadow_buffer_->textures()[0];
        break;
    case 6:
        screen_texture = direct_light_buffer_->textures()[0];
        break;
    case 7:
        screen_texture = probe_map_buffer_->textures()[0];
        target_sprite->set_pos(200, 0, screen_.width / 8, screen_.height);
        break;
    case 8:
        screen_texture = probe_map_buffer_->textures()[1];
        target_sprite->set_pos(200, 0, screen_.width / 8, screen_.height);
        break;
    case 9:
        screen_texture = light_map_lookup_buffer_->textures()[0];
        break;
    case 10:
        screen_texture = light_map_lookup_buffer_->textures()[1];
        break;
    case 11:
        screen_texture = direct_light_map_accumulation_buffer_->textures()[0];
        break;
    case 12:
        screen_texture = indirect_light_map_accumulation_buffer_->textures()[0];
        break;
    case 13:
        screen_texture = probe_buffer_->textures()[0];
        target_sprite->set_pos(200, 0, screen_.width / 8, screen_.height);
        break;
    case 14:
        screen_texture = probe_coefficients_buffer_->textures()[0];
        target_sprite->set_pos(200, 0, screen_.width / 8, screen_.height);
        break;
    case 15:
        screen_texture = indirect_light_buffer_->textures()[0];
        break;
    case 0:
    default:
        screen_texture = light_buffer_->textures()[0];
        break;
    }
    alt_screen_texture = geometry_buffer_->textures()[0];

    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);

    // Push the full screen quad used to render FBO
    target_sprite->Render(context_);

    // Set the inputs
    if (!sprite_shader_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !sprite_shader_->SetInput("sprite", screen_texture, context_))
    {
        return false;
    }

    if (!sprite_shader_->Render(target_sprite->index_count(), context_))
    {
        return false;
    }

    // Push the mini screen quad used to render alt FBO
    alt_target->Render(context_);

    // Set the inputs
    if (!sprite_shader_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !sprite_shader_->SetInput("sprite", alt_screen_texture, context_))
    {
        return false;
    }

    if (!sprite_shader_->Render(alt_target->index_count(), context_))
    {
        return false;
    }
    return true;
}

bool Graphics::RenderSprites()
{
    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);

    for (const auto& sprite : sprites_)
    {
        // Prep the pipeline 4 drawering
        sprite->Render(context_);

        // Set the inputs
        if (!sprite_shader_->SetInput("proj_matrix", ortho_matrix_, context_) ||
            !sprite_shader_->SetInput("sprite", sprite->texture(), context_))
        {
            return false;
        }

        // Make the draw call
        if (!sprite_shader_->Render(sprite->index_count(), context_))
        {
            return false;
        }
    }
    return true;
}

void Graphics::Reload(Client::Info screen)
{
    log::Debug("Reloading ... ");
    Timer timer;
    resource::ClearBufferCache();
    MakeContext(screen);
    for (auto& m : models_)
    {
        m->Reload(context_);
    }
    for (auto& s : sprites_)
    {
        s->Reload(context_);
    }
    log::Debug("%ims!\n", timer.ms());
    BuildLighting();
}

bool Graphics::BuildLighting()
{
    log::Debug("Building irradiance volumes... ");
    Timer timer;
    context_->BeginScene(Vector4(0, 0, 0, 1));
    // Mark the position and normal of every vertex on the lightmap
    if (!BuildLightMapLookups())
    {
        return false;
    }
    // Render out a cube map for every probe to sample scene data from (should only be called once per map load... put in make context?)
    if (!BuildProbeMaps())
    {
        return false;
    }
    log::Debug("%ims!\n", timer.ms());
    return true;
}

Camera* Graphics::camera() const
{
    return camera_.get();
}

gui::Manager* Graphics::gui() const
{
    return gui_.get();
}

bool Graphics::MakeContext(Client::Info screen)
{
    screen_ = screen;

    // OpenGL
    context_.reset();
    context_ = RenderContext(new RenderGL40(screen, kEnableVsync, (kRenderMode == RenderMode::FULLSCREEN)));
    if (!context_)
    {
        return false;
    }

    // Projection matrix (3D space->2D screen)
    float fov = kPi / 4.0f;
    float screen_aspect = static_cast<float>(screen.width) / static_cast<float>(screen.height);

    proj_matrix_ = MatrixPerspective(fov, screen_aspect, kScreenNear, kScreenDepth);

    // Ortho projection matrix (for 2d stuff, shadow maps, etc)
    ortho_matrix_ = MatrixOrthographic(0, units::pixel_to_subpixel(screen.width), units::pixel_to_subpixel(screen.height), 0,
                                       kScreenNear, kScreenDepth);

    light_map_ortho_matrix_ = MatrixOrthographic(0,
                                                 static_cast<units::world>(kLightMapResolution),
                                                 static_cast<units::world>(kLightMapResolution),
                                                 0, 0, 1);

    // Initialize our light probes...
    // TODO: Should be in map data somewhere, probably
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
    // Probe projection matrix (cube map perspective)
    // 90 degree FOV to make seems between edges add up perfectly to 360
    // Cube map aspect ratio, of course, is 1
    probe_proj_matrix_ = MatrixPerspective(kPi / 2, 1.0f, kScreenNear, kScreenDepth);
    // For when we do probe lookups and renders
    probe_ortho_matrix_ = MatrixOrthographic(0,
                                             static_cast<units::world>(6 * kProbeMapSize),
                                             static_cast<units::world>(probes_.size() * kProbeMapSize),
                                             0, 0, 1);
    // Shaders
    ShaderAttributeList geo_inputs;
    geo_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    geo_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    geo_inputs.push_back(ShaderAttribute(NORMAL, "input_norm"));
    geo_inputs.push_back(ShaderAttribute(TANGENT, "input_tan"));
    geo_inputs.push_back(ShaderAttribute(BITANGENT, "input_bitan"));
    geo_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/mesh.frag.glsl", geo_inputs, context_));

    ShaderAttributeList shadow_inputs;
    shadow_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    shadow_shader_.reset(new Shader("shaders/shadow.vert.glsl", "shaders/shadow.frag.glsl", shadow_inputs, context_));

    ShaderAttributeList blur_inputs;
    blur_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    blur_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    blur_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/blur.frag.glsl", blur_inputs, context_));

    ShaderAttributeList direct_light_inputs;
    direct_light_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    direct_light_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    direct_light_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/direct-light.frag.glsl", direct_light_inputs, context_));

    ShaderAttributeList direct_light_map_inputs;
    direct_light_map_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    direct_light_map_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    direct_light_map_shader_.reset(new Shader("shaders/direct-light-map.vert.glsl", "shaders/direct-light-map.frag.glsl", direct_light_map_inputs, context_));

    ShaderAttributeList indirect_light_inputs;
    indirect_light_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    indirect_light_inputs.push_back(ShaderAttribute(TEX, "input_uv")); // Stores probe id
    indirect_light_inputs.push_back(ShaderAttribute(NORMAL, "input_norm")); // Stores probe position
    indirect_light_shader_.reset(new Shader("shaders/indirect-light.vert.glsl", "shaders/indirect-light.frag.glsl", indirect_light_inputs, context_));

    ShaderAttributeList indirect_light_map_inputs;
    indirect_light_map_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    indirect_light_map_inputs.push_back(ShaderAttribute(TEX, "input_uv")); // Stores probe id
    indirect_light_map_inputs.push_back(ShaderAttribute(NORMAL, "input_norm")); // Stores probe position
    indirect_light_map_shader_.reset(new Shader("shaders/indirect-light-map.vert.glsl", "shaders/indirect-light-map.frag.glsl", indirect_light_map_inputs, context_));

    ShaderAttributeList light_map_lookup_inputs;
    light_map_lookup_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    light_map_lookup_inputs.push_back(ShaderAttribute(LIGHT_TEX, "input_light_uv"));
    light_map_lookup_inputs.push_back(ShaderAttribute(NORMAL, "input_norm"));
    light_map_lookup_shader_.reset(new Shader("shaders/light-map-lookup.vert.glsl", "shaders/light-map-lookup.frag.glsl", light_map_lookup_inputs, context_));

    ShaderAttributeList light_inputs;
    light_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    light_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    light_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/light.frag.glsl", light_inputs, context_));

    ShaderAttributeList sprite_inputs;
    sprite_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    sprite_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    sprite_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/sprite.frag.glsl", sprite_inputs, context_));

    ShaderAttributeList probe_map_inputs;
    probe_map_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    probe_map_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    probe_map_inputs.push_back(ShaderAttribute(LIGHT_TEX, "input_light_uv"));
    probe_map_shader_.reset(new Shader("shaders/probe-map.vert.glsl", "shaders/probe-map.frag.glsl", probe_map_inputs, context_));

    ShaderAttributeList probe_map_clear_inputs;
    probe_map_clear_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    probe_map_clear_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/probe-map-clear.frag.glsl", probe_map_clear_inputs, context_));

    ShaderAttributeList probe_inputs;
    probe_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    probe_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    probe_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/probe.frag.glsl", probe_inputs, context_));

    ShaderAttributeList probe_coefficients_inputs;
    probe_coefficients_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    probe_coefficients_shader_.reset(new Shader("shaders/sh-coefficients.vert.glsl", "shaders/sh-coefficients.frag.glsl", probe_coefficients_inputs, context_));

    ShaderAttributeList ui_inputs;
    ui_inputs.push_back(ShaderAttribute(POS, "input_pos"));
    ui_inputs.push_back(ShaderAttribute(TEX, "input_uv"));
    auto ui_shader = std::unique_ptr<Shader>(new Shader("shaders/sprite.vert.glsl", "shaders/ui.frag.glsl", ui_inputs, context_));

    if (geo_shader_ == nullptr ||
        shadow_shader_ == nullptr ||
        blur_shader_ == nullptr ||
        direct_light_shader_ == nullptr ||
        indirect_light_shader_ == nullptr ||
        direct_light_map_shader_ == nullptr ||
        indirect_light_map_shader_ == nullptr ||
        light_map_lookup_shader_ == nullptr ||
        light_shader_ == nullptr ||
        sprite_shader_ == nullptr ||
        probe_map_shader_ == nullptr ||
        probe_map_clear_shader_ == nullptr ||
        probe_shader_ == nullptr ||
        probe_coefficients_shader_ == nullptr ||
        ui_shader == nullptr)
    {
        return false;
    }

    // Framebuffers
    geometry_buffer_.reset(new Framebuffer(screen.width, screen.height, 4, context_));
    shadow_buffer_.reset(new Framebuffer(kShadowMapResolution, kShadowMapResolution, { { TextureHint::R16G16, TextureHint::LINEAR } }, context_));
    blur_buffer_.reset(new Framebuffer(kShadowMapResolution, kShadowMapResolution, { { TextureHint::R16G16, TextureHint::LINEAR } }, context_));
    direct_light_buffer_.reset(new Framebuffer(screen.width, screen.height, 1, false, context_));
    indirect_light_buffer_.reset(new Framebuffer(screen.width, screen.height, { { TextureHint::R32G32B32A32, TextureHint::LINEAR } }, false, context_));
    light_map_lookup_buffer_.reset(new Framebuffer(kLightMapResolution, kLightMapResolution, { { TextureHint::R32G32B32, TextureHint::NEAREST }, { TextureHint::R8G8B8, TextureHint::NEAREST } }, false, context_));
    direct_light_map_accumulation_buffer_.reset(new Framebuffer(kLightMapResolution, kLightMapResolution, { { TextureHint::R32G32B32A32, TextureHint::LINEAR } }, false, context_));
    indirect_light_map_accumulation_buffer_.reset(new Framebuffer(kLightMapResolution, kLightMapResolution, { { TextureHint::R32G32B32A32, TextureHint::LINEAR } }, false, context_));
    light_buffer_.reset(new Framebuffer(screen.width, screen.height, 1, false, context_));
    // TODO: Should tex map buffer be linearly sampled or nearest? (Currently linear)
    //     Linear would make smoother lightmaps
    //     Nearest would prevent bleeding between geometry edges
    probe_map_buffer_.reset(new Framebuffer(kProbeMapSize * 6, kProbeMapSize * static_cast<int>(probes_.size()), 2, context_));
    probe_buffer_.reset(new Framebuffer(kProbeMapSize * 6, kProbeMapSize * static_cast<int>(probes_.size()), { { TextureHint::R8G8B8, TextureHint::NEAREST } }, context_));
    probe_coefficients_buffer_.reset(new Framebuffer(9, static_cast<int>(probes_.size()), { { TextureHint::R8G8B8, TextureHint::NEAREST } }, context_));

    // GUI
    if (gui_ == nullptr)
    {
        gui_.reset(new gui::Manager(screen.width, screen.height, std::move(ui_shader), context_));
    }
    else
    {
        gui_->Reload(screen.width, screen.height, std::move(ui_shader), context_);
    }

    return true;
}

void ManagedModel::Finish()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
        deleter_ = nullptr;
    }

    mesh_.reset();
    albedo_texture_.reset();
    normal_texture_.reset();
    light_texture_.reset();
}

ManagedModel::~ManagedModel()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
    }
}

void ManagedSprite::Finish()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
        deleter_ = nullptr;
    }

    vertex_buffer_.reset();
    index_buffer_.reset();
    texture_.reset();
}

ManagedSprite::~ManagedSprite()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
    }
}
} // namespace blons
