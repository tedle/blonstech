#include "graphics/gui/textarea.h"

// Local Includes
#include "graphics/gui/label.h"

namespace blons
{
namespace GUI
{
Textarea::Textarea(Box pos, FontStyle style, Manager* parent_manager, Window* parent_window)
{
    pos_ = pos;
    font_style_ = style;
    gui_ = parent_manager;
    parent_ = parent_window;
}

void Textarea::Render(RenderContext& context)
{
    for (auto& line : lines_)
    {
        line->Render(context);
    }
}

bool Textarea::Update(const Input& input)
{
    return false;
}

void Textarea::AddLine(std::string text)
{
    lines_.push_back(std::unique_ptr<Label>(new Label(Vector2(pos_.x, pos_.y), text, font_style_, gui_, parent_)));
}
} // namespace GUI
} // namespace blons