#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

// Includes
#include <Windows.h>
// ---
#include "directx.h"

#define R_MODE_FULLSCREEN        1
#define R_MODE_WINDOW            2
#define R_MODE_BORDERLESS_WINDOW 3

const int WINDOW_MODE = R_MODE_WINDOW;
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