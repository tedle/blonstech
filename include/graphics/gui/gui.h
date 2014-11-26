#ifndef BLONSTECH_GRAPHICS_GUI_GUI_H_
#define BLONSTECH_GRAPHICS_GUI_GUI_H_

// Includes
#include <map>
// Local Includes
#include "input/inputtemp.h"
#include "graphics/render/drawbatcher.h"
#include "graphics/gui/control.h"
#include "graphics/gui/skin.h"
#include "graphics/gui/font.h"
#include "graphics/gui/label.h"
#include "graphics/gui/button.h"
#include "graphics/gui/window.h"

namespace blons
{
// Forward declarations
class Shader;

namespace GUI
{
// Forward declarations
class Label;
class Window;

class Manager
{
public:
    Manager(int screen_width, int screen_height, std::unique_ptr<Shader> ui_shader, RenderContext& context);
    ~Manager();

    // Wanted to name these all Create*(), but macros are really mean
    //Window* MakeWindow(std::string id, int x, int y, int width, int height);

    bool LoadFont(std::string filename, int pixel_size, RenderContext& context);
    bool LoadFont(std::string filename, FontType usage, int pixel_size, RenderContext& context);

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
    friend Window;
    void RegisterDrawCall(DrawCallInfo info, DrawBatcher* batch);
    Skin* skin() const;
    Window* active_window() const;
    void set_active_window(Window* window);
    Vector2 screen_dimensions();

    // Raw pointers because cleared every frame
    std::vector<std::pair<DrawCallInfo, DrawBatcher*>> draw_batches_;

    Vector2 screen_dimensions_;
    Matrix ortho_matrix_;

    std::unique_ptr<Shader> ui_shader_;

    std::unique_ptr<Skin> skin_;
    std::vector<std::unique_ptr<Window>> windows_;
    std::unique_ptr<Window> main_window_;
};
} // namespace GUI
} // namespace blons
#endif