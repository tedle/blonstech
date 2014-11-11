#ifndef BLONSTECH_GRAPHICS_H_
#define BLONSTECH_GRAPHICS_H_

// Includes
#include <memory>
#include <set>
#include <Windows.h>
// Local Includes
#include "graphics/model.h"
#include "graphics/sprite.h"
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

    std::unique_ptr<Model> CreateModel(const char* filename);
    std::unique_ptr<Sprite> CreateSprite(const char* filename);

    bool Render();

    class Camera* camera();

private:

    RenderContext context_;
    std::unique_ptr<class Camera> camera_;
    std::unique_ptr<class Shader> shader3d_;
    std::unique_ptr<class Shader> shader2d_;
    std::unique_ptr<class Shader> shader_font_;
    std::unique_ptr<class Font> font_;
    // Keeps track of generated models & sprites for rendering
    // Will automatically update when referenced resources are deleted, so go ham
    std::set<Model*> models_;
    std::set<Sprite*> sprites_;
};
} // namespace blons

#endif