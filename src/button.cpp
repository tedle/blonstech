#include "graphics/gui/button.h"

#include "graphics/gui/label.h"
namespace blons
{
namespace GUI
{
Button::Button(int x, int y, int width, int height, const char* label, Manager* parent_manager)
{
    pos_ = Box(static_cast<float>(x),
               static_cast<float>(y),
               static_cast<float>(width),
               static_cast<float>(height));
    label_ = std::unique_ptr<Label>(new Label(0, 0, label, parent_manager));
}

void Button::Render(RenderContext& context)
{

}

bool Button::Update(const Input& input)
{
    g_log->Debug("yo im a button\n");
    return false;
}
} // namespace GUI
} // namespace blons

