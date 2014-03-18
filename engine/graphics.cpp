#include "graphics.h"

std::unique_ptr<RenderAPI> g_render = nullptr;

Graphics::Graphics()
{
    g_render = nullptr;

    camera_ = nullptr;
    shader_ = nullptr;
}

Graphics::~Graphics()
{
}

bool Graphics::Init(int screen_width, int screen_height, HWND hwnd)
{
    // DirectX
    //g_render = std::unique_ptr<RenderAPI>(new RenderD3D11);

    // OpenGL
    g_render = std::unique_ptr<RenderAPI>(new RenderGL40);
    if (!g_render)
    {
        return false;
    }

    if (!g_render->Init(screen_width, screen_height, kEnableVsync, hwnd, (kRenderMode==kRenderModeFullscreen), kScreenDepth, kScreenNear))
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

    if (!models_[0]->Init("../notes/teapot_highpoly.mesh", "../notes/white.dds"))
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

    if (!models_[1]->Init("../notes/sponza/sponza.mesh", "../notes/white.dds"))
    {
        MessageBox(hwnd, L"Model die", L"help", MB_OK);
        return false;
    }
    models_[1]->SetPos(0.0, 0.0, 0.0);
    //models_ = load_codmap("../notes/castletest", std::move(models_));

    // Shaders
    shader_ = std::unique_ptr<Shader>(new Shader);
    if (shader_ == nullptr)
    {
        return false;
    }

    if (!shader_->Init(hwnd))
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
        model->Finish();
    }

    if (shader_)
    {
        shader_->Finish();
    }

    if (g_render)
    {
        g_render->Finish();
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
    g_render->BeginScene();

    // Update camera matrix
    camera_->Render();

    // Get matrices
    view_matrix       = camera_->GetViewMatrix();
    projection_matrix = g_render->GetProjectionMatrix();

    for (auto const& model : models_)
    {
        // Prep the pipeline 4 drawering
        model->Render();
        world_matrix = model->GetWorldMatrix();

        // Finally do the render
        if (!shader_->Render(model->GetIndexCount(), model->GetTexture(),
            world_matrix, view_matrix, projection_matrix))
        {
            return false;
        }
    }

    // Swap buffers
    g_render->EndScene();

    return true;
}
