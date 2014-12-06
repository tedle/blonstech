#ifndef BLONSTECH_GRAPHICS_GUI_BUTTON_H_
#define BLONSTECH_GRAPHICS_GUI_BUTTON_H_

// Local Includes
#include "graphics/gui/control.h"

namespace blons
{
namespace GUI
{
class Button : public Control
{
public:
    Button(Box pos, std::string label, Manager* parent_manager, Window* parent_window);
    ~Button() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

    void set_callback(std::function<void()> callback);

protected:
    void RenderBody(const Skin::Layout::ButtonSet::Button& b, RenderContext& context);

private:
    std::unique_ptr<class Label> label_;
    std::function<void()> callback_;
    bool hover_;
    bool active_;
};
} // namespace GUI
} // namespace blons
#endif