#include "graphics/gui/label.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
Label::Label(Vector2 pos, std::string text, FontStyle style, Manager* parent_manager, Window* parent_window)
{
    pos_ = Box(pos.x, pos.y, 0.0f, 0.0f);
    text_ = ColourString(text);
    colour_parsing_ = true;
    font_style_ = style;
    gui_ = parent_manager;
    parent_ = parent_window;
}

void Label::Render(RenderContext& context)
{
    auto font = gui_->skin()->font(font_style_);

    auto parent_pos = parent_->pos();
    units::subpixel x = pos_.x + parent_pos.x;
    units::subpixel y = pos_.y + parent_pos.y;
    if (colour_parsing_)
    {
        for (const auto& frag : text_.fragments())
        {
            // One draw call per (colour,font) used across all labels combined
            auto batcher = font_batch(font_style_, frag.colour, context);
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
        auto batcher = font_batch(font_style_, kDefaultTextColour, context);
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

void Label::set_text(ColourString text)
{
    text_ = text;
}

const ColourString& Label::text() const
{
    return text_;
}

void Label::set_colour_parsing(bool colour_parsing)
{
    colour_parsing_ = colour_parsing;
}
} // namespace GUI
} // namespace blons