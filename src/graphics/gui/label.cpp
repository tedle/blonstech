////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/graphics/gui/label.h>

// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
Label::Label(Vector2 pos, ColourString text, Skin::FontStyle style, Manager* parent_manager, Window* parent_window)
    : Control(Box(pos.x, pos.y, 0.0f, 0.0f), parent_manager, parent_window)
{
    text_ = text;
    colour_parsing_ = true;
    font_style_ = style;
}

void Label::Render()
{
    auto font = gui_->skin()->font(font_style_);

    auto parent_pos = parent_->pos();
    units::subpixel x = pos_.x + parent_pos.x;
    units::subpixel y = pos_.y + parent_pos.y;
    if (colour_parsing_)
    {
        for (const auto& frag : text_.fragments())
        {
            for (const auto& c : frag.text)
            {
                if (c == '\n')
                {
                    x = pos_.x + parent_pos.x;
                    y += font->line_height();
                    continue;
                }
                auto batch = font->BuildBatchInstance(c, x, y, crop_);
                if (batch != nullptr)
                {
                    gui_->SubmitFontBatch(batch->pos, batch->uv, font_style_, frag.colour, crop_, feather_);
                }
                x += font->advance();
            }
        }
    }
    else
    {
        for (const auto& c : text_.raw_str())
        {
            if (c == '\n')
            {
                x = pos_.x + parent_pos.x;
                y += font->line_height();
                continue;
            }
            auto batch = font->BuildBatchInstance(c, x, y, crop_);
            if (batch != nullptr)
            {
                gui_->SubmitFontBatch(batch->pos, batch->uv, font_style_, text_.base_colour(), crop_, feather_);
            }
            x += font->advance();
        }
    }
}

bool Label::Update(const Input& input)
{
    return false;
}

void Label::set_style(Skin::FontStyle style)
{
    font_style_ = style;
}

void Label::set_text(std::string text)
{
    text_ = ColourString(text, text_.base_colour());
}

void Label::set_text(ColourString text)
{
    text_ = text;
}

void Label::set_text_colour(Vector4 colour)
{
    text_.set_base_colour(colour);
}

const ColourString& Label::text() const
{
    return text_;
}

void Label::set_colour_parsing(bool colour_parsing)
{
    colour_parsing_ = colour_parsing;
}
} // namespace gui
} // namespace blons