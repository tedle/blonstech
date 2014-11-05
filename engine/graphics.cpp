#include "graphics.h"

// Local Includes
#include "renderd3d11.h"
#include "rendergl40.h"
#include "camera.h"
#include "math.h"
#include "model.h"
#include "shader.h"
#include "temphelpers.h"

namespace blons
{
Graphics::Graphics()
{
    context_ = nullptr;
    camera_ = nullptr;
    shader_ = nullptr;
}

Graphics::~Graphics()
{
}

bool Graphics::Init(int screen_width, int screen_height, HWND hwnd)
{
    // DirectX
    //context_ = RenderContext(new RenderD3D11);

    // OpenGL
    context_ = RenderContext(new RenderGL40);
    if (!context_)
    {
        return false;
    }

    if (!context_->Init(screen_width, screen_height, kEnableVsync, hwnd, (kRenderMode==kRenderModeFullscreen), kScreenDepth, kScreenNear))
    {
        g_log->Fatal("Renderer failed to initailize\n");
        return false;
    }

    // Camera
    camera_ = std::unique_ptr<Camera>(new Camera);
    if (camera_ == nullptr)
    {
        return false;
    }

    camera_->set_pos(0.0f, 0.0f, -10.0f);

    // Model 1
    models_.push_back(std::unique_ptr<Model>(new Model));
    if (models_[0] == nullptr)
    {
        return false;
    }

    if (!models_[0]->Load("../../notes/teapot_highpoly.bms", context_))
    {
        g_log->Fatal("FATAL: Teapot initialization procedures were unsuccessful\n");
        return false;
    }
    models_[0]->set_pos(0.0, 0.0, 20.0);
    // Model 2
    models_.push_back(std::unique_ptr<Model>(new Model));
    if (models_[1] == nullptr)
    {
        return false;
    }

    if (!models_[1]->Load("../../notes/cube.bms", context_))
    {
        g_log->Fatal("no cube :(\n");
        return false;
    }
    models_[1]->set_pos(10.0, 0.0, 20.0);
    models_ = load_codmap("../../notes/bms_test", std::move(models_), context_);

    // Shaders
    shader_ = std::unique_ptr<Shader>(new Shader);
    if (shader_ == nullptr)
    {
        return false;
    }

    if (!shader_->Load(hwnd, context_))
    {
        g_log->Fatal("Shaders failed to initialize\n");
        return false;
    }

    return true;
}

void Graphics::Finish()
{
    for (auto const& model : models_)
    {
        model->Finish(context_);
    }

    if (shader_)
    {
        shader_->Finish(context_);
    }

    if (context_)
    {
        context_->Finish();
    }

    return;
}

bool Graphics::Frame()
{
    if (!Render())
    {
        return false;
    }

    return true;
}

Camera* Graphics::camera()
{
    return camera_.get();
}

bool Graphics::Render()
{
    Matrix view_matrix, projection_matrix, world_matrix;

    // Clear buffers
    context_->BeginScene();

    // Update camera matrix
    camera_->Render();

    // Get matrices
    view_matrix       = camera_->view_matrix();
    projection_matrix = context_->projection_matrix();

    for (auto const& model : models_)
    {
        // Prep the pipeline 4 drawering
        model->Render(context_);
        world_matrix = model->world_matrix();

        // Finally do the render
        if (!shader_->Render(model->index_count(), model->texture(),
            world_matrix, view_matrix, projection_matrix, context_))
        {
            return false;
        }
    }

    // Swap buffers
    context_->EndScene();

    return true;
}
} // namespace blons
