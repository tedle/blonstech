#include "graphics/gui/window.h"
namespace blons
{
namespace GUI
{
Window::Window(int x, int y, int width, int height, WindowType type)
{
    pos_ = Box(static_cast<float>(x),
               static_cast<float>(y),
               static_cast<float>(width),
               static_cast<float>(height));
    type_ = type;
}

void Window::Render(RenderContext& context)
{

}

void Window::Update(const Input& input)
{

}
} // namespace GUI
} // namespace blons
