#ifndef BLONSTECH_GRAPHICS_GRAPHICS_H_
#define BLONSTECH_GRAPHICS_GRAPHICS_H_

// Includes
#include <memory>
#include <set>
#include <Windows.h>
// Public Includes
#include <blons/graphics/model.h>
#include <blons/graphics/sprite.h>
#include <blons/graphics/render/render.h>

namespace blons
{
enum RenderMode
{
    FULLSCREEN,
    WINDOW,
    BORDERLESS_WINDOW
};

const RenderMode   kRenderMode  = RenderMode::WINDOW;
const bool         kEnableVsync = false;
const units::world kScreenDepth = 10000.0f;
const units::world kScreenNear  = 0.1f;

// Forward declarations
namespace gui { class Manager; }
class Camera;
class Shader;

// TODO: Add sprite & model factories that inject this class's RenderContext
//           Change RenderContext to shared_ptr??
// TODO: Custom shader pipelines like
//           graphics->SetPipeline(enum GFX_PIPELINE_2D_SPRITES, vector<string> shader_files, func shader_inputs_callback)
// OR        graphics->SetPipeline(enum GFX_PIPELINE_2D_SPRITES, Shader* shader)
//              shader->SetInputCallback(std::function<void(Shader*)>)
//                  ^ but what about model world matrix??
class Graphics
{
public:
    Graphics(units::pixel screen_width, units::pixel screen_height, HWND hwnd);
    ~Graphics();

    std::unique_ptr<Model> MakeModel(std::string filename);
    std::unique_ptr<Sprite> MakeSprite(std::string filename);

    bool Render();

    Camera* camera() const;
    gui::Manager* gui() const;

private:
    // Managed assets let this class create and track models & sprites.
    // It will auto-render all assets created thru this class every frame until they are deleted
    // It will also clean up their buffers without destruction as the graphics class is shutting down
    class ManagedModel : public Model
    {
    public:
        ManagedModel(std::string filename, RenderContext& context) : Model(filename, context) {}
        ~ManagedModel() override;
    private:
        friend Graphics;
        void Finish();
        std::function<void(ManagedModel*)> deleter_;
    };

    class ManagedSprite : public Sprite
    {
    public:
        ManagedSprite(std::string filename, RenderContext& context) : Sprite(filename, context) {}
        ~ManagedSprite() override;
    private:
        friend Graphics;
        void Finish();
        std::function<void(ManagedSprite*)> deleter_;
    };

    RenderContext context_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<gui::Manager> gui_;
    std::unique_ptr<Shader> shader3d_;
    std::unique_ptr<Shader> shader2d_;

    Matrix proj_matrix_, ortho_matrix_;
    // Keeps track of generated models & sprites for rendering
    // Will automatically update when referenced resources are deleted, so go ham
    std::set<ManagedModel*> models_;
    std::set<ManagedSprite*> sprites_;
};
} // namespace blons

#endif