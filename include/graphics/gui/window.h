#ifndef BLONSTECH_GRAPHICS_GUI_WINDOW_H_
#define BLONSTECH_GRAPHICS_GUI_WINDOW_H_

// Local Includes
#include "graphics/gui/control.h"
#include "graphics/gui/button.h"
#include "graphics/gui/label.h"
#include "graphics/gui/textarea.h"
#include "graphics/gui/textbox.h"

namespace blons
{
namespace gui
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

    void Render(RenderContext& context) override;
    bool Update(const Input& input) override;

    Button* MakeButton(units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string label);
    Label* MakeLabel(units::pixel x, units::pixel y, std::string text);
    Textarea* MakeTextarea(units::pixel x, units::pixel y, units::pixel width, units::pixel height);
    Textbox* MakeTextbox(units::pixel x, units::pixel y, units::pixel width, units::pixel height);

    const std::string id() const;

protected:
    std::vector<std::unique_ptr<Control>> controls_;

    void RenderBody(RenderContext& context);

private:
    const std::string id_;
    WindowType type_;

    // For draggable windows
    bool dragging_;
    Vector2 drag_offset_;
    std::unique_ptr<Label> caption_;
};
} // namepsace GUI
} // namespace blons
#endif