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
class Window : public Control
{
public:
    // Only draggable windows can have a caption, so type is omitted for that constructor
    Window(int x, int y, int width, int height, WindowType type, Manager* parent_manager);
    Window(int x, int y, int width, int height, const char* caption, Manager* parent_manager);
    ~Window() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

    Button* CreateButton(int x, int y, int width, int height, const char* label);
    Label* CreateLabel(int x, int y, const char* text);

private:
    void Init(int x, int y, int width, int height, const char* caption, WindowType type, Manager* parent_manager);

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