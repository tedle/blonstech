#include "graphics/gui/button.h"

#include "graphics/gui/label.h"
namespace blons
{
namespace GUI
{
Button::Button(int x, int y, int width, int height, const char* label, Manager* parent_manager, Window* parent_window)
{
    pos_ = Box(static_cast<float>(x),
               static_cast<float>(y),
               static_cast<float>(width),
               static_cast<float>(height));
    label_ = std::unique_ptr<Label>(new Label(pos_.x, pos_.y, label, parent_manager, parent_window));
    gui_ = parent_manager;
    parent_ = parent_window;
}

void Button::Render(RenderContext& context)
{

}

bool Button::Update(const Input& input)
{
    return false;
}
} // namespace GUI
} // namespace blons

