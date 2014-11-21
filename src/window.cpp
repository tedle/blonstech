#include "graphics/gui/window.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
Window::Window(int x, int y, int width, int height, WindowType type, Manager* parent_manager)
{
    pos_ = Box(static_cast<float>(x),
               static_cast<float>(y),
               static_cast<float>(width),
               static_cast<float>(height));
    type_ = type;
    gui_ = parent_manager;
}

void Window::Render(RenderContext& context)
{
    // Render self
    auto skin = gui_->skin();
    auto layout = skin->layout();
    auto sprite = skin->sprite();
    sprite->set_pos(pos_);
    sprite->set_subtexture(layout->window);
    gui_->control_batch()->Append(*sprite->mesh());

    // Render controls
    for (const auto& control : controls_)
    {
        control->Render(context);
    }
}

void Window::Update(const Input& input)
{

}

Label* Window::CreateLabel(int x, int y, const char* text)
{
    std::unique_ptr<Label> label(new Label(x, y, text, gui_));
    controls_.push_back(std::move(label));
    return static_cast<Label*>(controls_.back().get());
}
} // namespace GUI
} // namespace blons
