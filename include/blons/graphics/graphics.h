////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef BLONSTECH_GRAPHICS_GRAPHICS_H_
#define BLONSTECH_GRAPHICS_GRAPHICS_H_

// Includes
#include <memory>
#include <set>
#include <Windows.h>
// Public Includes
#include <blons/graphics/camera.h>
#include <blons/graphics/framebuffer.h>
#include <blons/graphics/model.h>
#include <blons/graphics/sprite.h>
#include <blons/graphics/render/render.h>
#include <blons/system/client.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Determines how the system window should be setup for rendering
////////////////////////////////////////////////////////////////////////////////
enum RenderMode
{
    FULLSCREEN,       ///< GPU based fullscreen rendering
    WINDOW,           ///< Render inside a draggable window
    BORDERLESS_WINDOW ///< Render inside a window fit to the screen, hiding taskbar
};

////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for graphics creation
////////////////////////////////////////////////////////////////////////////////
const RenderMode kRenderMode = RenderMode::WINDOW;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for graphics creation
////////////////////////////////////////////////////////////////////////////////
const bool kEnableVsync = false;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for max render distance
////////////////////////////////////////////////////////////////////////////////
const units::world kScreenDepth = 100.0f;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for nearest render distance
////////////////////////////////////////////////////////////////////////////////
const units::world kScreenNear = 0.1f;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for shadow map resolution
////////////////////////////////////////////////////////////////////////////////
const units::pixel kShadowMapResolution = 1024;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for light map resolution
////////////////////////////////////////////////////////////////////////////////
const units::pixel kLightMapResolution = 512;

// Forward declarations
class Light;
class Shader;
namespace gui { class Manager; }

// TODO: Custom shader pipelines like
//           graphics->SetPipeline(enum GFX_PIPELINE_2D_SPRITES, vector<string> shader_files, func shader_inputs_callback)
// OR        graphics->SetPipeline(enum GFX_PIPELINE_2D_SPRITES, Shader* shader)
//              shader->SetInputCallback(std::function<void(Shader*)>)
//                  ^ but what about model world matrix??

// TODO: Refactor a lot of this into a pipeline/DeferredPipeline class

////////////////////////////////////////////////////////////////////////////////
/// \brief Manager for efficiently creating & rendering sprites, models, and UI
////////////////////////////////////////////////////////////////////////////////
class Graphics
{
public:
    // TODO: Decouple this from Win32 (pass client to constructor?)
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a graphics manager for a given client window
    ///
    /// \param screen Dimensions and handle of the client window
    ////////////////////////////////////////////////////////////////////////////////
    Graphics(Client::Info screen);
    ~Graphics();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new model from the given mesh file. Models created with
    /// this function are automatically rendered each frame until their memory is
    /// freed. Note that deleting the graphics class before the model will result
    /// in all of the model's buffers becoming invalid
    ///
    /// \param filename Location of the mesh file on disk
    /// \return Pointer to the created model, memory is owned by the caller
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Model> MakeModel(std::string filename);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new sprite from the given image file. Sprites created with
    /// this function are automatically rendered each frame until their memory is
    /// freed. Note that deleting the graphics class before the sprite will result
    /// in all of the sprite's buffers becoming invalid
    ///
    /// \param filename Location of the image file on disk
    /// \return Pointer to the created sprite, memory is owned by the caller
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Sprite> MakeSprite(std::string filename);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders all sprites, models, and UI elements created through this
    /// class to the screen, as well as handles any startup and teardown for a frame
    ///
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool Render();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads graphics API. Used for applying new video settings. **Never**
    /// call this in the middle of rendering a frame (callbacks beware!).
    ////////////////////////////////////////////////////////////////////////////////
    void Reload(Client::Info screen);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to build global illumination for the scene. Should be called
    /// **once** after all static geometry has been loaded.
    ///
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool BuildLighting();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Returns a pointer to the camera owned by the blons::Graphics manager
    /// that is used each frame to view renderable models
    ///
    /// \return Pointer to the camera
    ////////////////////////////////////////////////////////////////////////////////
    Camera* camera() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Returns a pointer to the gui::Manager owned by this class that is
    /// used for the creation and management of UI elements
    ///
    /// \return Pointer to the GUI manager
    ////////////////////////////////////////////////////////////////////////////////
    gui::Manager* gui() const;

private:
    bool MakeContext(Client::Info screen);

    // Should only be called once per map load
    bool RenderProbeMaps();

    bool RenderGeometry(Matrix view_matrix);
    bool RenderShadowMaps(Matrix view_matrix, Matrix light_vp_matrix);
    bool RenderLightMaps(Matrix light_vp_matrix);
    bool RenderLighting(Matrix view_matrix);
    bool RenderComposite();
    bool RenderSprites();

    RenderContext context_;
    Client::Info screen_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<gui::Manager> gui_;
    // TODO: Document the pipeline
    std::unique_ptr<Shader> geo_shader_;
    std::unique_ptr<Shader> shadow_shader_;
    std::unique_ptr<Shader> blur_shader_;
    std::unique_ptr<Shader> direct_light_shader_;
    std::unique_ptr<Shader> direct_light_map_shader_;
    std::unique_ptr<Shader> light_shader_;
    std::unique_ptr<Shader> sprite_shader_;
    std::unique_ptr<Framebuffer> geometry_buffer_;
    std::unique_ptr<Framebuffer> shadow_buffer_;
    std::unique_ptr<Framebuffer> blur_buffer_;
    std::unique_ptr<Framebuffer> direct_light_buffer_;
    std::unique_ptr<Framebuffer> light_map_accumulation_buffer_;
    std::unique_ptr<Framebuffer> light_buffer_;

    // Light probe stuff
    std::vector<Vector3> probes_;
    std::unique_ptr<Camera> probe_view_;
    std::unique_ptr<Shader> probe_map_shader_;
    std::unique_ptr<Shader> probe_map_clear_shader_;
    std::unique_ptr<Shader> probe_shader_;
    std::unique_ptr<Shader> probe_coefficients_shader_;
    std::unique_ptr<Framebuffer> probe_map_buffer_;
    std::unique_ptr<Framebuffer> probe_buffer_;
    std::unique_ptr<Framebuffer> probe_coefficients_buffer_;
    Matrix probe_proj_matrix_, probe_ortho_matrix_;
    const int kProbeMapSize = 16;

    Matrix proj_matrix_, ortho_matrix_;

    // TODO: Make this user customizable
    std::unique_ptr<Light> sun_;
    Vector3 sky_colour_;

    std::set<class ManagedModel*> models_;
    std::set<class ManagedSprite*> sprites_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Graphics
/// \ingroup graphics
///
/// Main class for users wanting to get quickly up and running with rendering.
/// 
/// ### Example:
/// \code
/// // Quick setup of a new window and bound graphics manager
/// auto client = std::make_unique<blons::Client>();
/// auto info = client->screen_info();
/// auto graphics = std::make_unique<blons::Graphics>(info.width, info.height, info.hwnd);
///
/// // Creating a model
/// auto model = graphics->MakeModel("model.bms");
/// // Moving the model
/// model->set_pos(10.0f, 10.0f, 10.0f);
///
/// // Creating a sprite
/// auto sprite = graphics->MakeSprite("sprite.png");
/// // Moving the sprite
/// sprite->set_pos(0, 0, 32, 32);
///
/// // Working with the GUI
/// auto gui = graphics->gui();
/// gui->MakeWindow("window_id", 450, 250, 300, 300, "Great window!");
///
/// // Render loop
/// bool quit = false;
/// while (!quit)
/// {
///     // Poll for input events
///     quit = client->Frame();
///
///     // Handle input logic for the UI
///     gui->Update(*client->input());
///
///     // Render all models, sprites, and UI elements
///     graphics->Render();
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GRAPHICS_H_