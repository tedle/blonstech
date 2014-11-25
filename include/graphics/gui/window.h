#ifndef BLONSTECH_GRAPHICS_GUI_WINDOW_H_
#define BLONSTECH_GRAPHICS_GUI_WINDOW_H_

// Local Includes
#include "graphics/gui/control.h"
#include "graphics/gui/button.h"
#include "graphics/gui/label.h"

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
class Window
{
public:
    Window(int x, int y, int width, int height, WindowType type, Manager* parent_manager);
    ~Window() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

    Button* CreateButton(int x, int y, int width, int height, const char* label);
    Label* CreateLabel(int x, int y, const char* text);

    void set_pos(float x, float y);
    Box pos() const;

private:
    Box pos_;
    Manager* gui_;
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