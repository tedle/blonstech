#include "graphics.h"

std::unique_ptr<RenderAPI> g_render = nullptr;

Graphics::Graphics()
{
    g_render = nullptr;

    camera_ = nullptr;
    model_ = nullptr;
    shader_ = nullptr;
}

Graphics::~Graphics()
{
}

bool Graphics::Init(int screen_width, int screen_height, HWND hwnd)
{
    // DirectX
    // g_render = std::unique_ptr<RenderAPI>(new RenderD3D11);

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

    // Model
    model_ = std::unique_ptr<Model>(new Model);
    if (model_ == nullptr)
    {
        return false;
    }

    if (!model_->Init(L"../notes/teapot_highpoly.mesh", L"../notes/me.dds"))
    {
        MessageBox(hwnd, L"Model die", L"help", MB_OK);
        return false;
    }

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
    if (model_)
    {
        model_->Finish();
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

    // Prep the pipeline 4 drawering
    model_->Render();
    world_matrix = model_->GetWorldMatrix();

    // Finally do the render
    if (!shader_->Render(model_->GetIndexCount(), model_->GetTexture(),
                         world_matrix, view_matrix, projection_matrix))
    {
        return false;
    }

    // Swap buffers
    g_render->EndScene();

    return true;
}
