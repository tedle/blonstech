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
    // TODO: TEMP DEBUG CODE. CLEAN THIS UP!!!
    static bool init = true;
    static const console::Variable* target = nullptr;
    if (init)
    {
        console::RegisterVariable("gfx:target", 0);
        target = console::var("gfx:target");
        init = false;
    }

    Matrix world_matrix, view_matrix;

    // Clear buffers
    context_->BeginScene();

    // Get matrices
    view_matrix = camera_->view_matrix();

    // 3D Rendering pass
    // Needed so models dont render over themselves
    context_->SetDepthTesting(true);

    // Bind the geometry framebuffer to render all models onto
    geometry_buffer_->Bind(context_);

    // TODO: 3D pass ->
    //      Render static world geo as batches without world matrix
    //      Render movable objects singularly with world matrix
    for (const auto& model : models_)
    {
        // Prep the pipeline 4 drawering
        model->Render(context_);
        world_matrix = model->world_matrix();

        // Set the inputs
        if (!shader3d_->SetInput("world_matrix", world_matrix, context_) ||
            !shader3d_->SetInput("view_matrix", view_matrix, context_) ||
            !shader3d_->SetInput("proj_matrix", proj_matrix_, context_) ||
            !shader3d_->SetInput("diffuse", model->diffuse(), 0, context_) ||
            !shader3d_->SetInput("normal", model->normal(), 1, context_))
        {
            return false;
        }

        // Finally do the render
        if (!shader3d_->Render(model->index_count(), context_))
        {
            return false;
        }
    }

    // Unbind the G-buffer, rebind the back buffer
    geometry_buffer_->Unbind(context_);

    // 2D Rendering pass
    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);

    // Render the geometry as a sprite
    geometry_buffer_->Render(context_);

    // Set the inputs
    if (!shader2d_->SetInput("world_matrix", MatrixIdentity() , context_) ||
        !shader2d_->SetInput("proj_matrix", ortho_matrix_, context_) ||
        !shader2d_->SetInput("diffuse", geometry_buffer_->textures()[target->to<int>()], context_))
    {
        return false;
    }

    // Finally do the render
    if (!shader2d_->Render(geometry_buffer_->index_count(), context_))
    {
        return false;
    }

    for (const auto& sprite : sprites_)
    {
        // Prep the pipeline 4 drawering
        sprite->Render(context_);

        // Set the inputs
        if (!shader2d_->SetInput("world_matrix", MatrixIdentity() , context_) ||
            !shader2d_->SetInput("proj_matrix", ortho_matrix_, context_) ||
            !shader2d_->SetInput("diffuse", sprite->texture(), context_))
        {
            return false;
        }

        // Finally do the render
        if (!shader2d_->Render(sprite->index_count(), context_))
        {
            return false;
        }
    }

    // 2D UI Pass
    gui_->Render(context_);

    // Swap buffers
    context_->EndScene();

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
    ShaderAttributeList inputs3d;
    inputs3d.push_back(ShaderAttribute(0, "input_pos"));
    inputs3d.push_back(ShaderAttribute(1, "input_uv"));
    inputs3d.push_back(ShaderAttribute(2, "input_norm"));
    inputs3d.push_back(ShaderAttribute(3, "input_tan"));
    inputs3d.push_back(ShaderAttribute(4, "input_bitan"));
    shader3d_.reset(new Shader("shaders/mesh.vert.glsl", "shaders/mesh.frag.glsl", inputs3d, context_));

    ShaderAttributeList inputs2d;
    inputs2d.push_back(ShaderAttribute(0, "input_pos"));
    inputs2d.push_back(ShaderAttribute(1, "input_uv"));
    shader2d_.reset(new Shader("shaders/sprite.vert.glsl", "shaders/sprite.frag.glsl", inputs2d, context_));

    ShaderAttributeList inputs_ui;
    inputs_ui.push_back(ShaderAttribute(0, "input_pos"));
    inputs_ui.push_back(ShaderAttribute(1, "input_uv"));
    auto ui_shader = std::unique_ptr<Shader>(new Shader("shaders/sprite.vert.glsl", "shaders/ui.frag.glsl", inputs_ui, context_));

    if (shader3d_ == nullptr ||
        shader2d_ == nullptr ||
        ui_shader == nullptr)
    {
        return false;
    }

    // Framebuffers
    geometry_buffer_.reset(new Framebuffer(screen.width, screen.height, 4, context_));

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
    diffuse_texture_.reset();
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
