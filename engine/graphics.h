#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

// Includes
#include <Windows.h>
// ---
#include "directx.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class CGraphics
{

public:
    CGraphics();
    ~CGraphics();

    bool Init(int, int, HWND);
    void Finish();
    bool Frame();

private:
    bool Render();

    CD3D* m_D3D;

};

#endif