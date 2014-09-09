#include "graphics.h"

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
        MessageBox(hwnd, L"Render die", L"help", MB_OK);
        return false;
    }

    // Camera
    camera_ = std::unique_ptr<Camera>(new Camera);
    if (camera_ == nullptr)
    {
        return false;
    }

    camera_->SetPos(0.0f, 0.0f, -10.0f);

    // Model 1
    models_.push_back(std::unique_ptr<Model>(new Model));
    if (models_[0] == nullptr)
    {
        return false;
    }

    if (!models_[0]->Init("../notes/teapot_highpoly.bms", context_))
    {
        MessageBox(hwnd, L"Model die", L"help", MB_OK);
        return false;
    }
    models_[0]->SetPos(0.0, 0.0, 20.0);
    // Model 2
    models_.push_back(std::unique_ptr<Model>(new Model));
    if (models_[1] == nullptr)
    {
        return false;
    }

    if (!models_[1]->Init("../notes/cube.bms", context_))
    {
        MessageBox(hwnd, L"Model die", L"help", MB_OK);
        return false;
    }
    models_[1]->SetPos(10.0, 0.0, 20.0);
    models_ = load_codmap("../notes/bms_test", std::move(models_), context_);

    // Shaders
    shader_ = std::unique_ptr<Shader>(new Shader);
    if (shader_ == nullptr)
    {
        return false;
    }

    if (!shader_->Init(hwnd, context_))
    {
        MessageBox(hwnd, L"Shaders die", L"help", MB_OK);
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

Camera* Graphics::GetCamera()
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
    view_matrix       = camera_->GetViewMatrix();
    projection_matrix = context_->GetProjectionMatrix();

    for (auto const& model : models_)
    {
        // Prep the pipeline 4 drawering
        model->Render(context_);
        world_matrix = model->GetWorldMatrix();

        // Finally do the render
        if (!shader_->Render(model->GetIndexCount(), model->GetTexture(),
            world_matrix, view_matrix, projection_matrix, context_))
        {
            return false;
        }
    }

    // Swap buffers
    context_->EndScene();

    return true;
}
