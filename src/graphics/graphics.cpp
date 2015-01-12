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

    camera_->set_pos(0.0f, 0.0f, -10.0f);

    // Sunlight
    // TODO: User should be doing this somehow
    sun_.reset(new Light(Light::DIRECTIONAL,
                         Vector3(20.0f, 20.0f, 5.0f),   // position
                         Vector3(-10.0f, -2.0f, -5.0f), // direction
                         Vector3(1.0f, 0.8f, 0.3f)));   // colour
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

    // Clear buffers
    context_->BeginScene();

    // Render all of the geometry and accompanying info (normal, depth, etc)
    if (!RenderGeometry(view_matrix))
    {
        return false;
    }

    // Render all of the geometry and get their depth from the light
    // Then render a shadow map from the depth information
    if (!RenderShadowMaps(view_matrix))
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

bool Graphics::RenderShadowMaps(Matrix view_matrix)
{
    // TODO: Parallel split shadow maps
    //     Shouldn't be much harder than splitting clip distance in ndc_box of sun_->ViewFrustum
    //     along a linear blend of logarithmic and uniform splits
    // Bind the shadow depth framebuffer to render all models onto
    shadow_buffer_->Bind(context_);

    Matrix light_vp_matrix = sun_->ViewFrustum(view_matrix * proj_matrix_, kScreenDepth);
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
        !direct_light_shader_->SetInput("light_depth", shadow_buffer_->textures()[0], 1, context_))
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

bool Graphics::RenderLighting(Matrix view_matrix)
{
    // Bind the buffer to do all lighting calculations on
    light_buffer_->Bind(context_);

    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);

    // Render the geometry as a sprite
    geometry_buffer_->Render(context_);

    // Used to turn pixel fragments into world coordinates
    Matrix inv_proj_view = MatrixInverse(view_matrix * proj_matrix_);

    // Set the inputs
    if (!light_shader_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !light_shader_->SetInput("inv_vp_matrix", inv_proj_view, context_) ||
        !light_shader_->SetInput("albedo", geometry_buffer_->textures()[0], 0, context_) ||
        !light_shader_->SetInput("normal", geometry_buffer_->textures()[1], 1, context_) ||
        !light_shader_->SetInput("depth", geometry_buffer_->depth(), 2, context_) ||
        !light_shader_->SetInput("direct_light", direct_light_buffer_->textures()[0], 3, context_) ||
        !light_shader_->SetInput("sun.dir", sun_->direction(), context_) ||
        // TODO: Better ambient and specular values
        !light_shader_->SetInput("sun.specular", sun_->colour() * sun_->colour(), context_) ||
        !light_shader_->SetInput("sun.ambient", sun_->colour() * 0.05f, context_) ||
        !light_shader_->SetInput("sun.colour", sun_->colour(), context_))
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
    static std::unique_ptr<Sprite> alt_target(new Sprite("blons:none", context_));
    if (init)
    {
        console::RegisterVariable("gfx:target", 0);
        target = console::var("gfx:target");
        init = false;
        alt_target->set_pos(580, 360, 200, 200);
        alt_target->set_subtexture(0, 0, 16, -16);
    }
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
        screen_texture = shadow_buffer_->depth();
        break;
    case 6:
        screen_texture = direct_light_buffer_->textures()[0];
        break;
    case 0:
    default:
        screen_texture = light_buffer_->textures()[0];
        break;
    }
    alt_screen_texture = shadow_buffer_->textures()[0];

    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);

    // Push the full screen quad used to render FBO
    light_buffer_->Render(context_);

    // Set the inputs
    if (!sprite_shader_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !sprite_shader_->SetInput("sprite", screen_texture, context_))
    {
        return false;
    }

    if (!sprite_shader_->Render(light_buffer_->index_count(), context_))
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
    // DirectX
    //context_ = RenderContext(new RenderD3D11);

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

    // Shaders
    ShaderAttributeList geo_inputs;
    geo_inputs.push_back(ShaderAttribute(0, "input_pos"));
    geo_inputs.push_back(ShaderAttribute(1, "input_uv"));
    geo_inputs.push_back(ShaderAttribute(2, "input_norm"));
    geo_inputs.push_back(ShaderAttribute(3, "input_tan"));
    geo_inputs.push_back(ShaderAttribute(4, "input_bitan"));
    geo_shader_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/mesh.frag.glsl", geo_inputs, context_));

    ShaderAttributeList shadow_inputs;
    shadow_inputs.push_back(ShaderAttribute(0, "input_pos"));
    shadow_shader_.reset(new Shader("shaders/shadow.vert.glsl", "shaders/shadow.frag.glsl", shadow_inputs, context_));

    ShaderAttributeList direct_light_inputs;
    direct_light_inputs.push_back(ShaderAttribute(0, "input_pos"));
    direct_light_inputs.push_back(ShaderAttribute(1, "input_uv"));
    direct_light_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/direct-light.frag.glsl", direct_light_inputs, context_));

    ShaderAttributeList light_inputs;
    light_inputs.push_back(ShaderAttribute(0, "input_pos"));
    light_inputs.push_back(ShaderAttribute(1, "input_uv"));
    light_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/light.frag.glsl", light_inputs, context_));

    ShaderAttributeList sprite_inputs;
    sprite_inputs.push_back(ShaderAttribute(0, "input_pos"));
    sprite_inputs.push_back(ShaderAttribute(1, "input_uv"));
    sprite_shader_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/sprite.frag.glsl", sprite_inputs, context_));

    ShaderAttributeList ui_inputs;
    ui_inputs.push_back(ShaderAttribute(0, "input_pos"));
    ui_inputs.push_back(ShaderAttribute(1, "input_uv"));
    auto ui_shader = std::unique_ptr<Shader>(new Shader("shaders/sprite.vert.glsl", "shaders/ui.frag.glsl", ui_inputs, context_));

    if (geo_shader_ == nullptr ||
        light_shader_ == nullptr ||
        sprite_shader_ == nullptr ||
        ui_shader == nullptr)
    {
        return false;
    }

    // Framebuffers
    geometry_buffer_.reset(new Framebuffer(screen.width, screen.height,
                                           { TextureFormat::R8G8B8, TextureFormat::R8G8B8,
                                             TextureFormat::R8G8B8, TextureFormat::R8G8B8 },
                                           context_));
    shadow_buffer_.reset(new Framebuffer(kShadowMapResolution, kShadowMapResolution, { TextureFormat::R16G16 }, context_));
    direct_light_buffer_.reset(new Framebuffer(screen.width, screen.height, { TextureFormat::R8G8B8 }, false, context_));
    light_buffer_.reset(new Framebuffer(screen.width, screen.height, { TextureFormat::R8G8B8 }, false, context_));

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
