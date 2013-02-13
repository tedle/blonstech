#include "graphics.h"

CGraphics::CGraphics()
{
    m_D3D = NULL;
}

CGraphics::~CGraphics()
{
}

bool CGraphics::Init(int screenWidth, int screenHeight, HWND hwnd)
{
    m_D3D = new CD3D;
    if(!m_D3D)
        return false;

    if(!m_D3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, (WINDOW_MODE==R_MODE_FULLSCREEN), SCREEN_DEPTH, SCREEN_NEAR))
    {
        MessageBox(hwnd, L"DirectX die", L"help", MB_OK);
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
    // Clear buffers
    m_D3D->BeginScene(0.5f, 0.0f, 0.5f, 1.0f);

    // Swap buffers
    m_D3D->EndScene();

    return true;
}