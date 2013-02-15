#include "graphics.h"

CGraphics::CGraphics()
{
    m_D3D = NULL;
    m_Camera = NULL;
    m_Model = NULL;
    m_Shader = NULL;
}

CGraphics::~CGraphics()
{
}

bool CGraphics::Init(int screenWidth, int screenHeight, HWND hwnd)
{
    // DirectX
    m_D3D = new CD3D;
    if(!m_D3D)
        return false;

    if(!m_D3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, (WINDOW_MODE==R_MODE_FULLSCREEN), SCREEN_DEPTH, SCREEN_NEAR))
    {
        MessageBox(hwnd, L"DirectX die", L"help", MB_OK);
        return false;
    }

    // Camera
    m_Camera = new CCamera;
    if(!m_Camera)
        return false;

    m_Camera->SetPos(XMFLOAT3(0.0f, 0.0f, -10.0f * 0.017f));

    // Model
    m_Model = new CModel;
    if(!m_Model)
        return false;

    if(!m_Model->Init(m_D3D->GetDevice()))
    {
        MessageBox(hwnd, L"Model die", L"help", MB_OK);
        return false;
    }

    // Shaders
    m_Shader = new CShader;
    if(!m_Shader)
        return false;

    if(!m_Shader->Init(m_D3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Shaders die", L"help", MB_OK);
        return false;
    }

    return true;
}

void CGraphics::Finish()
{
    if(m_D3D)
    {
        m_D3D->Finish();
        delete m_D3D;
        m_D3D = NULL;
    }

    if(m_Camera)
    {
        delete m_Camera;
        m_Camera = NULL;
    }

    if(m_Model)
    {
        m_Model->Finish();
        delete m_Model;
        m_Model = NULL;
    }

    if(m_Shader)
    {
        m_Shader->Finish();
        delete m_Shader;
        m_Shader = NULL;
    }

    return;
}

bool CGraphics::Frame()
{
    if(!Render())
        return false;

    return true;
}

bool CGraphics::Render()
{
    XMMATRIX viewMatrix, projectionMatrix, worldMatrix;

    // Clear buffers
    m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Update camera matrix
    m_Camera->Render();

    // Get matrices
    viewMatrix       = m_Camera->GetViewMatrix();
    worldMatrix      = m_D3D->GetWorldMatrix();
    projectionMatrix = m_D3D->GetProjectionMatrix();

    // Prep the pipeline 4 drawering
    m_Model->Render(m_D3D->GetDeviceContext());

    // Finally do the render
    if(!m_Shader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
        return false;

    // Swap buffers
    m_D3D->EndScene();

    return true;
}