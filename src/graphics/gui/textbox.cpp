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

#include <blons/graphics/gui/textbox.h>

// Includes
#include <algorithm>
// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
Textbox::Textbox(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window)
    : Control(pos, parent_manager, parent_window)
{
    active_ = false;
    drag_highlighting_ = false;
    text_ = "";
    font_style_ = style;
    // Empty lambda is easier than worrying about nullptrs
    callback_ = [](Textbox* t){};

    padding_ = units::subpixel_to_pixel(gui_->skin()->layout()->textbox.normal.left.w * 2);
    // For vertically centering the text
    const auto& font = gui_->skin()->font(font_style_);
    std::size_t letter_height = font->letter_height();
    Vector2 text_pos;
    text_pos.x = pos.x + padding_;
    text_pos.y = pos.y + floor((pos.h + letter_height) / 2);
    text_label_.reset(new Label(text_pos, text_, style, gui_, parent_));
    text_label_->set_colour_parsing(false);
    // Must be called after label is initialized
    SetCursorPos(text_.end());
    highlight_ = cursor_;
}

void Textbox::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    const Skin::Layout::Textbox* textbox_layout;
    if (active_)
    {
        textbox_layout = &layout->textbox.active;
    }
    else
    {
        textbox_layout = &layout->textbox.normal;
    }

    RenderBody(*textbox_layout, context);
    if (active_)
    {
        RenderCursor(layout->textbox.cursor, context);
    }

    RenderText(context);
}

void Textbox::RenderBody(const Skin::Layout::Textbox& t, RenderContext& context)
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

    // Label base colour
    text_label_->set_text_colour(t.colour);
}

void Textbox::RenderCursor(const Box& cursor, RenderContext& context)
{
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;
    auto crop = Box(x + padding_ / 2, 0.0f, pos_.w - padding_, 0.0f);
    auto feather = 0;
    auto sprite = gui_->skin()->sprite();
    auto batch = gui_->control_batch(crop, feather, context);

    if (cursor_ != highlight_)
    {
        auto x_offset = LabelOffset(cursor_);
        auto highlight_offset = LabelOffset(highlight_);
        // TODO: Make negative width sprites display reverse instead of getting culled?
        if (x_offset > highlight_offset)
        {
            std::swap(x_offset, highlight_offset);
        }
        auto cursor_width = highlight_offset - x_offset;
        auto cursor_height = gui_->skin()->font(font_style_)->letter_height() + 6.0f;
        auto y_offset = floor((pos_.h - cursor_height) / 2);
        sprite->set_pos(x + x_offset,
                        y + y_offset,
                        cursor_width,
                        cursor_height);
        sprite->set_subtexture(cursor);
        batch->Append(sprite->mesh(), context);
    }
    else if (cursor_blink_.ms() % 1000 < 500)
    {
        auto cursor_width = 1.0f;
        auto cursor_height = gui_->skin()->font(font_style_)->letter_height() + 6.0f;
        auto x_offset = LabelOffset(cursor_);
        auto y_offset = floor((pos_.h - cursor_height) / 2);
        sprite->set_pos(x + x_offset,
                        y + y_offset,
                        cursor_width,
                        cursor_height);
        sprite->set_subtexture(cursor);
        batch->Append(sprite->mesh(), context);
    }
}

void Textbox::RenderText(RenderContext& context)
{
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;

    text_label_->set_crop(Box(x + padding_ / 2, 0.0f, pos_.w - padding_, 0.0f), padding_ / 2);
    text_label_->Render(context);
}

bool Textbox::Update(const Input& input)
{
    auto mods = input.modifiers();
    auto events = GetEventsWithRepeats(input);

    UpdateHighlightScroll(input);

    for (const auto& e : events)
    {
        if (e.type == Input::Event::MOUSE_DOWN)
        {
            OnMouseDown(input);
        }
        else if (e.type == Input::Event::MOUSE_UP)
        {
            OnMouseUp(input);
        }
        else if (e.type == Input::Event::MOUSE_MOVE_X ||
                 e.type == Input::Event::MOUSE_MOVE_Y)
        {
            OnMouseMove(input);
        }

        if (active_)
        {
            auto key = static_cast<Input::KeyCode>(e.value);
            mods.Update(e);

            if (e.type == Input::Event::KEY_DOWN)
            {
                OnKeyDown(input, key, mods);
            }
            else if (e.type == Input::Event::KEY_UP)
            {
                OnKeyUp(input, key, mods);
            }
        }
    }

    // Consume input while active
    return active_;
}

void Textbox::set_callback(std::function<void(Textbox*)> callback)
{
    callback_ = callback;
}

bool Textbox::focus() const
{
    return active_;
}

void Textbox::set_focus(bool focus)
{
    if (focus)
    {
        active_ = true;
        cursor_blink_.start();
    }
    else
    {
        active_ = false;
        cursor_blink_.stop();
        key_repeat_.timer.stop();
        key_repeat_.code = Input::BAD;
    }
}

std::pair<std::size_t, std::size_t> Textbox::highlight() const
{
    return std::make_pair(std::distance<std::string::const_iterator>(text_.begin(), cursor_),
                          std::distance<std::string::const_iterator>(text_.begin(), highlight_));
}

// TODO: Add an overload that accepts iterators to avoid fix the whole size mismatch issue here
// Careful work to avoid underflows and such here since we're mixing ints and size_ts (sorry)
void Textbox::set_highlight(int cursor, int end)
{
    std::size_t cursor_index, end_index;
    if (cursor < 0)
    {
        if (abs(cursor) >= text_.length())
        {
            cursor_index = 0;
        }
        else
        {
            cursor_index = std::min(text_.length(), text_.length() + cursor + 1);
        }
    }
    else
    {
        cursor_index = std::min(text_.length(), static_cast<std::size_t>(cursor));
    }
    if (end < 0)
    {
        if (abs(cursor) >= text_.length())
        {
            end_index = 0;
        }
        else
        {
            end_index = std::min(text_.length(), text_.length() + end + 1);
        }
    }
    else
    {
        end_index = std::min(text_.length(), static_cast<std::size_t>(end));
    }

    SetCursorPos(text_.begin() + cursor_index);
    highlight_ = text_.begin() + end_index;
}

std::string Textbox::text() const
{
    return text_;
}

void Textbox::set_text(std::string text)
{
    text_ = text;
    text_label_->set_text(text_);
    SetCursorPos(text_.end());
    highlight_ = cursor_;
    drag_highlighting_ = false;
}

std::vector<Input::Event> Textbox::GetEventsWithRepeats(const Input& input)
{
    // Make a mutable copy so we can inject repeated key presses
    auto events(input.event_queue());
    if (key_repeat_.timer.ms() > 500)
    {
        auto code = key_repeat_.code;
        if (input.IsPrintable(code) ||
            code == Input::BACKSPACE ||
            code == Input::DEL ||
            code == Input::UP ||
            code == Input::DOWN ||
            code == Input::LEFT ||
            code == Input::RIGHT)
        {
            Input::Event repeat(Input::Event::KEY_DOWN, code);
            events.push_back(repeat);
            // How long before next repeated key press
            key_repeat_.timer.rewind(40);
        }
    }
    return std::move(events);
}

void Textbox::OnMouseDown(const Input& input)
{
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    units::pixel mx = input.mouse_x();
    units::pixel my = input.mouse_y();

    // Clicked inside textbox
    if (mx >= x && mx < x + pos_.w &&
        my >= y && my < y + pos_.h)
    {
        set_focus(true);
        drag_highlighting_ = true;

        SetCursorPos(NearestCursorPos(mx));
        highlight_ = cursor_;
    }
    else
    {
        set_focus(false);
    }
}

void Textbox::OnMouseUp(const Input& input)
{
    drag_highlighting_ = false;
}

void Textbox::OnMouseMove(const Input& input)
{
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    units::pixel mx = input.mouse_x();

    // Update highlight when dragging inside textbox
    if (mx >= x + padding_ && mx < x + pos_.w - padding_ &&
        drag_highlighting_)
    {
        SetCursorPos(NearestCursorPos(mx));
    }
}

void Textbox::OnKeyDown(const Input& input, const Input::KeyCode key, Input::Modifiers mods)
{
    // Easier to follow when it's moving around
    cursor_blink_.start();

    if (key != key_repeat_.code)
    {
        key_repeat_.timer.start();
        key_repeat_.code = key;
    }

    if (input.IsPrintable(key) && !mods.ctrl && !mods.alt)
    {
        if (cursor_ != highlight_)
        {
            // std::string::erase cannot erase backwards for some reason
            if (cursor_ > highlight_)
            {
                std::swap(cursor_, highlight_);
            }
            SetCursorPos(text_.erase(cursor_, highlight_));
        }
        SetCursorPos(text_.insert(cursor_, input.ToAscii(key, mods.shift)) + 1);
        highlight_ = cursor_;
    }
    else if (key == Input::BACKSPACE)
    {
        if (cursor_ != highlight_)
        {
            // std::string::erase cannot erase backwards for some reason
            if (cursor_ > highlight_)
            {
                std::swap(cursor_, highlight_);
            }
            SetCursorPos(text_.erase(cursor_, highlight_));
        }
        else if (cursor_ > text_.begin())
        {
            SetCursorPos(text_.erase(cursor_ - 1));
        }
        highlight_ = cursor_;
    }
    else if (key == Input::DEL)
    {
        if (cursor_ != highlight_)
        {
            // std::string::erase cannot erase backwards for some reason
            if (cursor_ > highlight_)
            {
                std::swap(cursor_, highlight_);
            }
            SetCursorPos(text_.erase(cursor_, highlight_));
        }
        else if (cursor_ < text_.end())
        {
            SetCursorPos(text_.erase(cursor_));
        }
        highlight_ = cursor_;
    }
    else if (key == Input::LEFT)
    {
        if (cursor_ > text_.begin())
        {
            SetCursorPos(cursor_ - 1);
        }
        if (!mods.shift)
        {
            highlight_ = cursor_;
        }
    }
    else if (key == Input::RIGHT)
    {
        if (cursor_ < text_.end())
        {
            SetCursorPos(cursor_ + 1);
        }
        if (!mods.shift)
        {
            highlight_ = cursor_;
        }
    }
    else if (key == Input::RETURN)
    {
        callback_(this);
    }
    text_label_->set_text(text_);
}

void Textbox::OnKeyUp(const Input& input, const Input::KeyCode key, Input::Modifiers mods)
{
    if (key == key_repeat_.code)
    {
        key_repeat_.timer.stop();
        key_repeat_.code = Input::BAD;
    }
    // Restart timer if we let up a key that's not being repeated
    else
    {
        key_repeat_.timer.start();
    }
}

std::function<void(Textbox*)> Textbox::callback() const
{
    return callback_;
}

Skin::FontStyle Textbox::font_style() const
{
    return font_style_;
}

Label* Textbox::label() const
{
    return text_label_.get();
}

units::pixel Textbox::padding() const
{
    return padding_;
}

std::string::iterator Textbox::NearestCursorPos(units::pixel mouse_x)
{
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;

    auto cursor_pos = text_.end();
    units::pixel str_width = 0;
    const auto font = gui_->skin()->font(font_style_);
    const auto label_offset = text_label_->pos().x - pos_.x;

    // Loop thru text by each letter, accumulating pixel width until it exceeds mouse pos
    for (auto it = text_.begin(); it != text_.end(); it++)
    {
        auto cur_width = font->string_width(std::string(1, *it), false);
        str_width += cur_width;
        // Clicked on left half of char?
        if (str_width - cur_width / 2 > mouse_x - x - label_offset)
        {
            cursor_pos = it;
            break;
        }
        // Clicked on right half of char?
        else if (str_width > mouse_x - x - label_offset)
        {
            cursor_pos = it + 1;
            break;
        }
    }
    // Return pos of one we found or default to end of line
    return cursor_pos;
}

void Textbox::SetCursorPos(std::string::iterator cursor)
{
    cursor_ = cursor;
    const auto label_pos = text_label_->pos();
    const auto cursor_offset = LabelOffset(cursor_);
    const auto end_of_text_offset = LabelOffset(text_.end());
    // Cursor out of bounds right side
    if (cursor_offset >= pos_.w - padding_)
    {
        auto label_diff = cursor_offset - (pos_.w - padding_) + 1;
        text_label_->set_pos(label_pos.x - label_diff, label_pos.y);
    }
    // Cursor out of bounds left side
    else if (cursor_offset < padding_)
    {
        auto label_diff = cursor_offset - padding_;
        text_label_->set_pos(label_pos.x - label_diff, label_pos.y);
    }
    // Label is offscreen left && whitespace exists on the right
    else if (label_pos.x - (pos_.x + padding_) < 0 && end_of_text_offset < pos_.w - padding_)
    {
        auto label_diff = end_of_text_offset - (pos_.w - padding_) + 1;
        // std::min prevents whitespace appearing on the left side of the text
        text_label_->set_pos(std::min(label_pos.x - label_diff, pos_.x + padding_), label_pos.y);
    }
}

units::subpixel Textbox::LabelOffset(std::string::iterator cursor)
{
    const auto font = gui_->skin()->font(font_style_);
    const auto layout = gui_->skin()->layout();
    const auto label_offset = text_label_->pos().x - (pos_.x + padding_);
    const auto cursor_offset = font->string_width(std::string(text_.begin(), cursor), false) + label_offset + padding_;
    return cursor_offset;
}

void Textbox::UpdateHighlightScroll(const Input& input)
{
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    units::pixel mx = input.mouse_x();

    units::subpixel distance_to_border = std::max((x + padding_) - mx, mx - (x + pos_.w - padding_));
    // Scroll faster when farther away from border of textbox
    units::time::ms scroll_wait_time = static_cast<units::time::ms>(200.0f / std::max(pow(distance_to_border, 0.7f), 1.0f));

    // Update highlight when dragging outside textbox
    if (drag_highlighting_ &&
        distance_to_border > 0 &&
        drag_highlight_scrolling_.ms() > scroll_wait_time)
    {
        if (mx < x + padding_ && cursor_ > text_.begin())
        {
            SetCursorPos(cursor_ - 1);
        }
        else if (mx >= x + pos_.w - padding_ && cursor_ < text_.end())
        {
            SetCursorPos(cursor_ + 1);
        }
        drag_highlight_scrolling_.start();
    }
}
} // namespace gui
} // namespace blons
