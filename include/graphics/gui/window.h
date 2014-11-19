#ifndef BLONSTECH_GRAPHICS_GUI_WINDOW_H_
#define BLONSTECH_GRAPHICS_GUI_WINDOW_H_

#include "graphics/gui/control.h"

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
    Window(int x, int y, int width, int height, WindowType type);
    ~Window() {}

    void Render(RenderContext& context);
    void Update(const Input& input);

private:
    WindowType type_;
    std::vector<Control> controls_;
};
} // namepsace GUI
} // namespace blons
#endif