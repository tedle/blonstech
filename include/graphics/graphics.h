#ifndef BLONSTECH_GRAPHICS_H_
#define BLONSTECH_GRAPHICS_H_

// Includes
#include <memory>
#include <vector>
#include <Windows.h>
// Local Includes
#include "render/render.h"

namespace blons
{
const int kRenderModeFullscreen       = 1;
const int kRenderModeWindow           = 2;
const int kRenderModeBorderlessWindow = 3;

const int   kRenderMode  = kRenderModeWindow;
const bool  kEnableVsync = false;
const float kScreenDepth = 10000.0f;
const float kScreenNear  = 0.1f;

// TODO: Add sprite & model factories that inject this class's RenderContext
//           Change RenderContext to shared_ptr??
// TODO: Custom shader pipelines like
//           graphics->SetPipeline(enum GFX_PIPELINE_2D_SPRITES, vector<string> shader_files, func shader_inputs_callback)
class Graphics
{

public:
    Graphics(int screen_width, int screen_height, HWND hwnd);
    ~Graphics();

    bool Render();

    class Camera* camera();

private:

    RenderContext context_;
    std::unique_ptr<class Camera> camera_;
    std::unique_ptr<class Shader> shader3d_;
    std::unique_ptr<class Shader> shader2d_;
    std::unique_ptr<class Shader> shader_font_;
    std::unique_ptr<class Font> font_;
    std::vector<std::unique_ptr<class Model>> models_;

};
} // namespace blons

#endif