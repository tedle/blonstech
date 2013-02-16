#include "graphics.h"

Graphics::Graphics()
{
    d3d_ = nullptr;
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
    d3d_ = new D3D;
    if (!d3d_)
        return false;

    if (!d3d_->Init(screen_width, screen_height, kEnableVsync, hwnd, (kRenderMode==kRenderModeFullscreen), kScreenDepth, kScreenNear))
    {
        MessageBox(hwnd, L"DirectX die", L"help", MB_OK);
        return false;
    }

    // Camera
    camera_ = new Camera;
    if (!camera_)
        return false;

    camera_->SetPos(XMFLOAT3(0.0f, 0.0f, -10.0f));

    // Model
    model_ = new Model;
    if (!model_)
        return false;

    if (!model_->Init(d3d_->GetDevice(), L"../.notes/me.dds"))
    {
        MessageBox(hwnd, L"Model die", L"help", MB_OK);
        return false;
    }

    // Shaders
    shader_ = new Shader;
    if (!shader_)
        return false;

    if (!shader_->Init(d3d_->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Shaders die", L"help", MB_OK);
        return false;
    }

    return true;
}

void Graphics::Finish()
{
    if (d3d_)
    {
        d3d_->Finish();
        delete d3d_;
        d3d_ = nullptr;
    }

    if (camera_)
    {
        delete camera_;
        camera_ = nullptr;
    }

    if (model_)
    {
        model_->Finish();
        delete model_;
        model_ = nullptr;
    }

    if (shader_)
    {
        shader_->Finish();
        delete shader_;
        shader_ = nullptr;
    }

    return;
}

bool Graphics::Frame()
{
    if (!Render())
        return false;

    return true;
}

bool Graphics::Render()
{
    XMFLOAT4X4 view_matrix, projection_matrix, world_matrix;

    // Clear buffers
    d3d_->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Update camera matrix
    camera_->Render();

    // Get matrices
    view_matrix       = camera_->GetViewMatrix();
    world_matrix      = d3d_->GetWorldMatrix();
    projection_matrix = d3d_->GetProjectionMatrix();

    // Prep the pipeline 4 drawering
    model_->Render(d3d_->GetDeviceContext());

    // Finally do the render
    if (!shader_->Render(d3d_->GetDeviceContext(), model_->GetIndexCount(),
                         world_matrix, view_matrix, projection_matrix,
                         model_->GetTexture()))
        return false;

    // Swap buffers
    d3d_->EndScene();

    return true;
}