#ifndef BLONSTECH_GRAPHICS_GUI_GUI_H_
#define BLONSTECH_GRAPHICS_GUI_GUI_H_

// Public Includes
#include <blons/math/animation.h>
#include <blons/input/inputtemp.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/gui/skin.h>
#include <blons/graphics/gui/font.h>
#include <blons/graphics/gui/window.h>
#include <blons/graphics/gui/consolewindow.h>

namespace blons
{
// Forward declarations
class Shader;

namespace gui
{
// Forward declarations
class Label;
class Window;

class Manager
{
public:
    Manager(units::pixel screen_width, units::pixel screen_height, std::unique_ptr<Shader> ui_shader, RenderContext& context);
    ~Manager();

    bool LoadFont(std::string filename, units::pixel pixel_size, RenderContext& context);
    bool LoadFont(std::string filename, units::pixel pixel_size, FontStyle style, RenderContext& context);

    // TODO: Maybe simplify these prototypes somehow?
    // Wanted to name these all Create*(), but macros are really mean
    Window* MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption);
    Window* MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, WindowType type);
    Window* MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption, WindowType type);

    void Render(RenderContext& context);
    // Returns true if GUI had input to handle, false otherwise
    bool Update(const Input& input);

    Window* window(std::string id);

private:
    // Since we want this class to be accessed by user, we hide these functions
    // despite widgets needing access to them. Kind of hacky to friend it up, but oh well
    friend Control;
    friend Button;
    friend Label;
    friend Textarea;
    friend ConsoleTextarea;
    friend Textbox;
    friend ConsoleTextbox;
    friend Window;
    friend ConsoleWindow;
    void RegisterDrawCall(DrawCallInputs info, DrawBatcher* batch);
    Skin* skin() const;
    Window* active_window() const;
    void set_active_window(Window* window);
    Box screen_dimensions();

    // Raw pointers because cleared every frame
    std::vector<std::pair<DrawCallInputs, DrawBatcher*>> draw_batches_;

    Box screen_dimensions_;
    Matrix ortho_matrix_;

    std::unique_ptr<Shader> ui_shader_;

    std::unique_ptr<Skin> skin_;
    std::vector<std::unique_ptr<Window>> windows_;
    std::unique_ptr<Window> main_window_;
    std::unique_ptr<Window> console_window_;
};
} // namespace gui
} // namespace blons
#endif