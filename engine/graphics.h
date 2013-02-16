#ifndef BLONSTECH_GRAPHICS_H_
#define BLONSTECH_GRAPHICS_H_

// Includes
#include <Windows.h>
// Local Includes
#include "directx.h"
#include "camera.h"
#include "model.h"
#include "shader.h"

const int kRenderModeFullscreen       = 1;
const int kRenderModeWindow           = 2;
const int kRenderModeBorderlessWindow = 3;

const int   kRenderMode  = kRenderModeBorderlessWindow;
const bool  kEnableVsync = false;
const float kScreenDepth = 1000.0f;
const float kScreenNear  = 0.1f;

class Graphics
{

public:
    Graphics();
    ~Graphics();

    bool Init(int, int, HWND);
    void Finish();
    bool Frame();

private:
    bool Render();

    D3D*    d3d_;
    Camera* camera_;
    Model*  model_;
    Shader* shader_;

};

#endif