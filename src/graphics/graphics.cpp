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
#include <blons/graphics/camera.h>
#include <blons/graphics/gui/gui.h>
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

    // 3D Rendering pass
    // Needed so models dont render over themselves
    context_->SetDepthTesting(true);

    // Bind the geometry framebuffer to render all models onto
    geometry_buffer_->Bind(context_);
    // Render all of the geometry and accompanying info (normal, depth, etc)
    if (!RenderGeometry(view_matrix))
    {
        return false;
    }

    // 2D Rendering pass
    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);

    // Bind the buffer to do all lighting calculations on
    light_buffer_->Bind(context_);
    if (!RenderLighting(view_matrix))
    {
        return false;
    }
    // Unbind the light buffer, rebind the back buffer
    light_buffer_->Unbind(context_);

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
    // TODO: 3D pass ->
    //      Render static world geo as batches without world matrix
    //      Render movable objects singularly with world matrix
    for (const auto& model : models_)
    {
        // Bind the vertex data
        model->Render(context_);

        // Set the inputs
        if (!shadergeo_->SetInput("world_matrix", model->world_matrix(), context_) ||
            !shadergeo_->SetInput("view_matrix", view_matrix, context_) ||
            !shadergeo_->SetInput("proj_matrix", proj_matrix_, context_) ||
            !shadergeo_->SetInput("albedo", model->albedo(), 0, context_) ||
            !shadergeo_->SetInput("normal", model->normal(), 1, context_))
        {
            return false;
        }

        // Make the draw call
        if (!shadergeo_->Render(model->index_count(), context_))
        {
            return false;
        }
    }
    return true;
}

bool Graphics::RenderLighting(Matrix view_matrix)
{
    // Render the geometry as a sprite
    geometry_buffer_->Render(context_);

    // Used to turn pixel fragments into world coordinates
    Matrix inv_proj_view = MatrixInverse(MatrixMultiply(view_matrix, proj_matrix_));

    // Set the inputs
    if (!shaderlight_->SetInput("world_matrix", MatrixIdentity() , context_) ||
        !shaderlight_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !shaderlight_->SetInput("inv_proj_view_matrix", inv_proj_view, context_) ||
        !shaderlight_->SetInput("albedo", geometry_buffer_->textures()[0], 0, context_) ||
        !shaderlight_->SetInput("normal", geometry_buffer_->textures()[1], 1, context_) ||
        !shaderlight_->SetInput("depth", geometry_buffer_->depth(), 2, context_) ||
        !shaderlight_->SetInput("sun.dir", Vector3Normalize(Vector3(-10.0, -2.0, -5.0)), context_) ||
        !shaderlight_->SetInput("sun.specular", Vector3(1.0f, 0.5f, 0.1f), context_) ||
        !shaderlight_->SetInput("sun.ambient", Vector3(0.1f, 0.08f, 0.08f), context_) ||
        !shaderlight_->SetInput("sun.colour", Vector3(1.0f, 0.8f, 0.3f), context_))
    {
        return false;
    }

    // Finally do the render
    if (!shaderlight_->Render(geometry_buffer_->index_count(), context_))
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
    if (init)
    {
        console::RegisterVariable("gfx:target", 0);
        target = console::var("gfx:target");
        init = false;
    }
    const TextureResource* screen_texture;
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
    case 0:
    default:
        screen_texture = light_buffer_->textures()[0];
        break;
    }

    // Push the full screen quad used to render FBO
    light_buffer_->Render(context_);

    // Set the inputs
    if (!shader2d_->SetInput("world_matrix", MatrixIdentity() , context_) ||
        !shader2d_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !shader2d_->SetInput("sprite", screen_texture, context_))
    {
        return false;
    }

    if (!shader2d_->Render(light_buffer_->index_count(), context_))
    {
        return false;
    }
    return true;
}

bool Graphics::RenderSprites()
{
    for (const auto& sprite : sprites_)
    {
        // Prep the pipeline 4 drawering
        sprite->Render(context_);

        // Set the inputs
        if (!shader2d_->SetInput("world_matrix", MatrixIdentity() , context_) ||
            !shader2d_->SetInput("proj_matrix", ortho_matrix_, context_) ||
            !shader2d_->SetInput("sprite", sprite->texture(), context_))
        {
            return false;
        }

        // Make the draw call
        if (!shader2d_->Render(sprite->index_count(), context_))
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

    proj_matrix_ = MatrixPerspectiveFov(fov, screen_aspect, kScreenNear, kScreenDepth);

    // Ortho projection matrix (for 2d stuff, shadow maps, etc)
    ortho_matrix_ = MatrixOrthographic(units::pixel_to_subpixel(screen.width), units::pixel_to_subpixel(screen.height),
                                       kScreenNear, kScreenDepth);

    // Shaders
    ShaderAttributeList inputsgeo;
    inputsgeo.push_back(ShaderAttribute(0, "input_pos"));
    inputsgeo.push_back(ShaderAttribute(1, "input_uv"));
    inputsgeo.push_back(ShaderAttribute(2, "input_norm"));
    inputsgeo.push_back(ShaderAttribute(3, "input_tan"));
    inputsgeo.push_back(ShaderAttribute(4, "input_bitan"));
    shadergeo_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/mesh.frag.glsl", inputsgeo, context_));

    ShaderAttributeList inputslight;
    inputslight.push_back(ShaderAttribute(0, "input_pos"));
    inputslight.push_back(ShaderAttribute(1, "input_uv"));
    shaderlight_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/light.frag.glsl", inputslight, context_));

    ShaderAttributeList inputs2d;
    inputs2d.push_back(ShaderAttribute(0, "input_pos"));
    inputs2d.push_back(ShaderAttribute(1, "input_uv"));
    shader2d_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/sprite.frag.glsl", inputs2d, context_));

    ShaderAttributeList inputs_ui;
    inputs_ui.push_back(ShaderAttribute(0, "input_pos"));
    inputs_ui.push_back(ShaderAttribute(1, "input_uv"));
    auto ui_shader = std::unique_ptr<Shader>(new Shader("shaders/sprite.vert.glsl", "shaders/ui.frag.glsl", inputs_ui, context_));

    if (shadergeo_ == nullptr ||
        shaderlight_ == nullptr ||
        shader2d_ == nullptr ||
        ui_shader == nullptr)
    {
        return false;
    }

    // Framebuffers
    geometry_buffer_.reset(new Framebuffer(screen.width, screen.height, 4, context_));
    light_buffer_.reset(new Framebuffer(screen.width, screen.height, 1, context_));

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
