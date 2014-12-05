#include "graphics/gui/textarea.h"

// Local Includes
#include "graphics/gui/gui.h"
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

void Textarea::GenLabel(std::string text)
{
    const auto& font = gui_->skin()->font(font_style_);
    auto lines = font->string_wrap(text, units::subpixel_to_pixel(pos_.w));
    for (const auto& line : lines)
    {
        auto label = std::unique_ptr<Label>(new Label(Vector2(pos_.x, pos_.y + font->line_height() * lines_.size()), line, font_style_, gui_, parent_));
        lines_.push_back(std::move(label));
    }
}

void Textarea::AddLine(std::string text)
{
    history_.push_back(text);
    GenLabel(text);
}

void Textarea::Clear()
{
    history_.clear();
    lines_.clear();
}
} // namespace GUI
} // namespace blons