#include <blons/graphics/graphics.h>

// Public Includes
#include <blons/graphics/model.h>
#include <blons/graphics/sprite.h>
#include <blons/graphics/camera.h>
#include <blons/graphics/gui/gui.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/render/shader.h>
#include <blons/math/math.h>
#include <blons/temphelpers.h>
// Local Includes
#include "render/renderd3d11.h"
#include "render/rendergl40.h"

namespace blons
{
Graphics::Graphics(units::pixel screen_width, units::pixel screen_height, HWND hwnd)
{
    context_ = nullptr;
    camera_ = nullptr;
    shader3d_ = nullptr;
    shader2d_ = nullptr;

    // DirectX
    //context_ = RenderContext(new RenderD3D11);

    // OpenGL
    context_ = RenderContext(new RenderGL40(screen_width, screen_height, kEnableVsync, hwnd,
                                            (kRenderMode == RenderMode::FULLSCREEN)));
    if (!context_)
    {
        log::Fatal("Renderer failed to initailize\n");
        throw "Failed to initialize rendering context";
    }

    // Projection matrix (3D space->2D screen)
    float fov = kPi / 4.0f;
    float screen_aspect = static_cast<float>(screen_width) / static_cast<float>(screen_height);

    proj_matrix_ = MatrixPerspectiveFov(fov, screen_aspect, kScreenNear, kScreenDepth);

    // Ortho projection matrix (for 2d stuff, shadow maps, etc)
    ortho_matrix_ = MatrixOrthographic(units::pixel_to_subpixel(screen_width), units::pixel_to_subpixel(screen_height),
                                       kScreenNear, kScreenDepth);

    // Camera
    camera_ = std::unique_ptr<Camera>(new Camera);
    if (camera_ == nullptr)
    {
        throw "Failed to initialize camera";
    }

    camera_->set_pos(0.0f, 0.0f, -10.0f);

    // Shaders
    ShaderAttributeList inputs3d;
    inputs3d.push_back(ShaderAttribute(0, "input_pos"));
    inputs3d.push_back(ShaderAttribute(1, "input_uv"));
    inputs3d.push_back(ShaderAttribute(2, "input_norm"));
    shader3d_ = std::unique_ptr<Shader>(new Shader("mesh.vert.glsl", "mesh.frag.glsl", inputs3d, context_));

    ShaderAttributeList inputs2d;
    inputs2d.push_back(ShaderAttribute(0, "input_pos"));
    inputs2d.push_back(ShaderAttribute(1, "input_uv"));
    shader2d_ = std::unique_ptr<Shader>(new Shader("sprite.vert.glsl", "sprite.frag.glsl", inputs2d, context_));

    ShaderAttributeList inputs_ui;
    inputs_ui.push_back(ShaderAttribute(0, "input_pos"));
    inputs_ui.push_back(ShaderAttribute(1, "input_uv"));
    auto ui_shader = std::unique_ptr<Shader>(new Shader("sprite.vert.glsl", "ui.frag.glsl", inputs_ui, context_));

    if (shader3d_ == nullptr ||
        shader2d_ == nullptr ||
        ui_shader == nullptr)
    {
        log::Fatal("Shaders failed to initialize\n");
        throw "Failed to initialize shader";
    }

    // GUI
    gui_ = std::unique_ptr<gui::Manager>(new gui::Manager(screen_width, screen_height, std::move(ui_shader), context_));
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
    Matrix world_matrix, view_matrix;

    // Clear buffers
    context_->BeginScene();

    // Update camera matrix
    camera_->Render();

    // Get matrices
    view_matrix = camera_->view_matrix();

    // 3D Rendering pass
    // Needed so models dont render over themselves
    context_->SetDepthTesting(true);
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
            !shader3d_->SetInput("diffuse", model->texture(), context_))
        {
            return false;
        }

        // Finally do the render
        if (!shader3d_->Render(model->index_count(), context_))
        {
            return false;
        }
    }

    // 2D Rendering pass
    // Needed so sprites can render over themselves
    context_->SetDepthTesting(false);
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

Camera* Graphics::camera() const
{
    return camera_.get();
}

gui::Manager* Graphics::gui() const
{
    return gui_.get();
}

void Graphics::ManagedModel::Finish()
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

Graphics::ManagedModel::~ManagedModel()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
    }
}

void Graphics::ManagedSprite::Finish()
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

Graphics::ManagedSprite::~ManagedSprite()
{
    if (deleter_ != nullptr)
    {
        deleter_(this);
    }
}
} // namespace blons
