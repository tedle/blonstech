////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
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

#include <blons/graphics/gui/textarea.h>

// Includes
#include <algorithm>
// Public Includes
#include <blons/math/animation.h>
#include <blons/graphics/gui/gui.h>
#include <blons/graphics/gui/label.h>

namespace blons
{
namespace gui
{
Textarea::Textarea(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window)
    : Control(pos, parent_manager, parent_window)
{
    font_style_ = style;

    newest_top_ = false;
    edge_width_ = units::subpixel_to_pixel(gui_->skin()->layout()->textarea.left.w);
    padding_ = gui_->skin()->layout()->textarea.padding + edge_width_;

    scroll_offset_ = 0;
    scroll_destination_ = 0;
}

void Textarea::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    RenderBody(layout->textarea, context);
    RenderText(layout->textarea, context);
}

void Textarea::RenderBody(const Skin::Layout::Textarea& t, RenderContext& context)
{
    auto sprite = gui_->skin()->sprite();
    auto batch = gui_->control_batch(crop_, feather_, context);
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    // Top left corner
    sprite->set_pos(x,
                    y,
                    t.top_left.w,
                    t.top_left.h);
    sprite->set_subtexture(t.top_left);
    batch->Append(sprite->mesh(), context);

    // Top edge
    sprite->set_pos(x + t.top_left.w,
                    y,
                    pos_.w - (t.top_left.w + t.top_right.w),
                    t.top.h);
    sprite->set_subtexture(t.top);
    batch->Append(sprite->mesh(), context);

    // Top right corner
    sprite->set_pos(x + pos_.w - t.top_right.w,
                    y,
                    t.top_right.w,
                    t.top_right.h);
    sprite->set_subtexture(t.top_right);
    batch->Append(sprite->mesh(), context);

    // Left edge
    sprite->set_pos(x,
                    y + t.top_left.h,
                    t.left.w,
                    pos_.h - (t.top_left.h + t.bottom_right.h));
    sprite->set_subtexture(t.left);
    batch->Append(sprite->mesh(), context);

    // Body
    sprite->set_pos(x + t.left.w,
                    y + t.top.h,
                    pos_.w - (t.left.w + t.right.w),
                    pos_.h - (t.top.h + t.bottom.h));
    sprite->set_subtexture(t.body);
    batch->Append(sprite->mesh(), context);

    // Right edge
    sprite->set_pos(x + pos_.w - t.right.w,
                    y + t.top_right.h,
                    t.right.w,
                    pos_.h - (t.top_right.h + t.bottom_right.h));
    sprite->set_subtexture(t.right);
    batch->Append(sprite->mesh(), context);

    // Bottom left corner
    sprite->set_pos(x,
                    y + pos_.h - t.bottom_left.h,
                    t.bottom_left.w,
                    t.bottom_left.h);
    sprite->set_subtexture(t.bottom_left);
    batch->Append(sprite->mesh(), context);

    // Bottom edge
    sprite->set_pos(x + t.bottom_left.w,
                    y + pos_.h - t.bottom.h,
                    pos_.w - (t.bottom_left.w + t.bottom_right.w),
                    t.bottom.h);
    sprite->set_subtexture(t.bottom);
    batch->Append(sprite->mesh(), context);

    // Bottom right corner
    sprite->set_pos(x + pos_.w - t.bottom_right.w,
                    y + pos_.h - t.bottom_right.h,
                    t.bottom_right.w,
                    t.bottom_right.h);
    sprite->set_subtexture(t.bottom_right);
    batch->Append(sprite->mesh(), context);
}

void Textarea::RenderText(const Skin::Layout::Textarea& t, RenderContext& context)
{
    auto parent_pos = parent_->pos();
    auto y = pos_.y + parent_pos.y;

    // Update scroll position
    scroll_animation_.Update();

    // Render the text, only grab lines that are at visible
    const auto& font = gui_->skin()->font(font_style_);
    units::pixel line_height = font->line_height();
    auto line_offset = scroll_offset_ / line_height;
    // You only need a height of 2 pixels to be able to see 2 lines, so thats minimum
    std::size_t renderable_lines = std::min(lines_.size() - line_offset,
                                            static_cast<std::size_t>((pos_.h / line_height) + 2));

    for (auto i = line_offset; i < renderable_lines + line_offset; i++)
    {
        const auto feather = padding_ - edge_width_;
        auto& label = lines_[lines_.size() - i - 1];
        Box crop(0.0f, y + edge_width_, 0.0f, pos_.h - edge_width_ * 2);
        // Don't feather our g's and y's if we're at the bottom
        // Divide by line_height gives us a smoother transition
        crop.h += std::max(0, feather - scroll_offset_);
        label->set_crop(crop, feather);

        if (newest_top_)
        {
            label->set_pos(pos_.x + padding_, pos_.y + padding_ + i * line_height);
        }
        else
        {
            label->set_pos(pos_.x + padding_, pos_.y + pos_.h - padding_ - i * line_height + scroll_offset_);
        }

        label->set_text_colour(t.colour);
        label->Render(context);
    }
}

bool Textarea::Update(const Input& input)
{
    bool input_handled = false;

    auto us = scroll_timer_.us();
    for (const auto& e : input.event_queue())
    {
        // TODO: Make a more sensible if block
        if (gui_->active_window() == parent_)
        {
            if (e.type == Input::Event::KEY_DOWN)
            {
                const auto& font = gui_->skin()->font(font_style_);
                const auto ph = units::subpixel_to_pixel(pos_.h);
                auto page_offset = std::max(font->line_height(), ph - (ph % font->line_height()));
                if (e.value == Input::KeyCode::PG_UP)
                {
                    MoveScrollOffset(page_offset, true);
                    input_handled = true;
                }
                else if (e.value == Input::KeyCode::PG_DOWN)
                {
                    MoveScrollOffset(-page_offset, true);
                    input_handled = true;
                }
            }
        }
        // Handle these even when unfocused, so long as mouse pointer is inside textarea
        if (e.type == Input::Event::MOUSE_SCROLL)
        {
            auto parent_pos = parent_->pos();
            auto x = pos_.x + parent_pos.x;
            auto y = pos_.y + parent_pos.y;

            units::pixel mx = input.mouse_x();
            units::pixel my = input.mouse_y();

            // Mouse inside textarea
            if (mx >= x && mx < x + pos_.w &&
                my >= y && my < y + pos_.h)
            {
                const auto& font = gui_->skin()->font(font_style_);
                MoveScrollOffset(font->line_height() * e.value * 3, true);
            }
        }
    }

    return input_handled;
}

void Textarea::GenLabel(std::string text)
{
    const auto& font = gui_->skin()->font(font_style_);
    // Are we appending to already printed text?
    if (text_.length() > 0 && text_.back() != '\n')
    {
        assert(lines_.size() > 0);
        text = lines_.back()->text().raw_str() + text;
        lines_.pop_back();
    }
    auto lines = font->string_wrap(ColourString(text), units::subpixel_to_pixel(pos_.w) - padding_ * 2);
    if (newest_top_)
    {
        for (auto line = lines.rbegin(); line != lines.rend(); line++)
        {
            auto label = std::unique_ptr<Label>(new Label(Vector2(0, 0), *line, font_style_, gui_, parent_));
            lines_.push_back(std::move(label));
        }
    }
    else
    {
        for (const auto& line : lines)
        {
            auto label = std::unique_ptr<Label>(new Label(Vector2(0, 0), line, font_style_, gui_, parent_));
            lines_.push_back(std::move(label));
        }
    }
}

void Textarea::AddText(std::string text)
{
    auto old_size = lines_.size();
    GenLabel(text);
    text_ += text;

    bool at_origin = scroll_destination_ == 0;
    auto line_height = gui_->skin()->font(font_style_)->line_height();
    auto new_lines_height = static_cast<units::pixel>((lines_.size() - old_size) * line_height);
    // Set current scroll offset to look the same as before lines were added
    MoveScrollOffset(new_lines_height, false);
    if (at_origin)
    {
        // If we were at 0 before AddLine was called, smoothly animate in the new lines
        scroll_offset_ = new_lines_height;
        MoveScrollOffset(-scroll_offset_, true);
    }
}

void Textarea::AddLine(std::string text)
{
    AddText(text + '\n');
}

void Textarea::Clear()
{
    text_.clear();
    lines_.clear();

    scroll_offset_ = 0;
    scroll_destination_ = 0;
    scroll_animation_.Stop();
}

void Textarea::MoveScrollOffset(units::pixel delta, bool smooth)
{
    const auto& font = gui_->skin()->font(font_style_);
    units::pixel max_offset = font->line_height() * static_cast<units::pixel>(lines_.size())
                            - units::subpixel_to_pixel(pos_.h)
                            + font->letter_height()
                            - edge_width_;

    scroll_destination_ = std::max(0, std::min(scroll_destination_ + delta, max_offset));

    if (!smooth)
    {
        scroll_offset_ = scroll_destination_;
    }
    // Create smooth scroll animation
    auto scroll_start = scroll_offset_;
    Animation::Callback callback = [this, scroll_start](float y)
    {
        scroll_offset_ = scroll_start + units::subpixel_to_pixel(round((scroll_destination_ - scroll_start) * y));
    };
    // Duration becomes linearly shorter when less than 30 pixels from completion
    units::time::ms duration = static_cast<units::time::ms>(300 * (std::min(abs(scroll_destination_ - scroll_offset_), 30) / 30.0f));
    if (!smooth)
    {
        duration = 0;
    }
    scroll_animation_ = Animation(duration, callback, Animation::CUBIC_OUT);
}
} // namespace gui
} // namespace blons