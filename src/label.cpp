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
    colour_parsing_ = true;
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
    if (colour_parsing_)
    {
        for (const auto& frag : text_.fragments())
        {
            // One draw call per (colour,font) used across all labels combined
            auto batcher = font_batch(font_type_, frag.colour, context);
            for (const auto& c : frag.text)
            {
                auto sprite = font->BuildSprite(c, x, y, crop_);
                if (sprite != nullptr)
                {
                    batcher->Append(*sprite->mesh());
                }
                x += font->advance();
            }
        }
    }
    else
    {
        auto batcher = font_batch(font_type_, kDefaultTextColour, context);
        for (const auto& c : text_.raw_str())
        {
            auto sprite = font->BuildSprite(c, x, y, crop_);
            if (sprite != nullptr)
            {
                batcher->Append(*sprite->mesh());
            }
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

void Label::set_colour_parsing(bool colour_parsing)
{
    colour_parsing_ = colour_parsing;
}
} // namespace GUI
} // namespace blons