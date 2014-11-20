#ifndef BLONSTECH_GRAPHICS_GUI_WINDOW_H_
#define BLONSTECH_GRAPHICS_GUI_WINDOW_H_

// Local Includes
#include "graphics/gui/control.h"
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
    Window(int x, int y, int width, int height, WindowType type, Manager* parent_manager);
    ~Window() {}

    void Render(RenderContext& context);
    void Update(const Input& input);

    Label* CreateLabel(int x, int y, const char* text);

private:
    WindowType type_;
    std::vector<std::unique_ptr<Control>> controls_;
};
} // namepsace GUI
} // namespace blons
#endif