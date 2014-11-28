#include "graphics/gui/label.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
Label::Label(Vector2 pos, std::string text, Manager* parent_manager, Window* parent_window)
{
    Init(pos, text, FontType::LABEL, parent_manager, parent_window);
}

Label::Label(Vector2 pos, std::string text, FontType font_type, Manager* parent_manager, Window* parent_window)
{
    Init(pos, text, font_type, parent_manager, parent_window);
}

void Label::Init(Vector2 pos, std::string text, FontType font_type, Manager* parent_manager, Window* parent_window)
{
    pos_ = Box(pos.x, pos.y, 0, 0);
    text_ = ColourString(text);
    font_type_ = font_type;
    gui_ = parent_manager;
    parent_ = parent_window;
}

void Label::Render(RenderContext& context)
{
    auto font = gui_->skin()->font(font_type_);

    auto parent_pos = parent_->pos();
    int x = static_cast<int>(pos_.x + parent_pos.x);
    int y = static_cast<int>(pos_.y + parent_pos.y);
    for (const auto& frag : text_.fragments())
    {
        // One draw call per (colour,font) used across all labels combined
        auto batcher = font_batch(font_type_, frag.colour, context);
        for (const auto& c : frag.text)
        {
            batcher->Append(*font->BuildSprite(c, x, y)->mesh());
            x += font->advance();
        }
    }

    RegisterBatches();
}

bool Label::Update(const Input& input)
{
    return false;
}

void Label::set_text(std::string text)
{
    text_ = ColourString(text);
}
} // namespace GUI
} // namespace blons