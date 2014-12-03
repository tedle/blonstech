#ifndef BLONSTECH_GRAPHICS_GUI_WINDOW_H_
#define BLONSTECH_GRAPHICS_GUI_WINDOW_H_

// Local Includes
#include "graphics/gui/control.h"
#include "graphics/gui/button.h"
#include "graphics/gui/label.h"
#include "graphics/gui/textbox.h"

namespace blons
{
namespace GUI
{
enum WindowType
{
    DRAGGABLE,
    STATIC,
    INVISIBLE
};
class Window : public Control
{
public:
    Window(std::string id, Box pos, std::string caption, WindowType type, Manager* parent_manager);
    Window(std::string id, Box pos, WindowType type, Manager* parent_manager)
        : Window(id, pos, "", type, parent_manager) {}
    Window(std::string id, Box pos, std::string caption, Manager* parent_manager)
        : Window(id, pos, caption, WindowType::DRAGGABLE, parent_manager) {}
    ~Window() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

    Button* MakeButton(int x, int y, int width, int height, std::string label);
    Label* MakeLabel(int x, int y, std::string text);
    Textbox* MakeTextbox(int x, int y, int width, int height);

    const std::string id() const;

private:
    const std::string id_;
    WindowType type_;
    std::vector<std::unique_ptr<Control>> controls_;

    // For draggable windows
    bool dragging_;
    Vector2 drag_offset_;
    std::unique_ptr<Label> caption_;
};
} // namepsace GUI
} // namespace blons
#endif