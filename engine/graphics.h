#ifndef BLONSTECH_GRAPHICS_H_
#define BLONSTECH_GRAPHICS_H_

// Includes
#include <Windows.h>
// Local Includes
#include "renderd3d11.h"
#include "math.h"
#include "camera.h"
#include "model.h"
#include "shader.h"

const int kRenderModeFullscreen       = 1;
const int kRenderModeWindow           = 2;
const int kRenderModeBorderlessWindow = 3;

const int   kRenderMode  = kRenderModeWindow;
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

    Camera* GetCamera();

private:
    bool Render();

    Camera*    camera_;
    Model*     model_;
    Shader*    shader_;

};

#endif