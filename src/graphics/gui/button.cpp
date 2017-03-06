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

#include <blons/graphics/gui/button.h>

// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
Button::Button(Box pos, std::string label, Manager* parent_manager, Window* parent_window)
    : Control(pos, parent_manager, parent_window)
{
    hover_ = false;
    active_ = false;
    // Empty lambda is easier than worrying about nullptrs
    callback_ = [](){};

    Vector2 caption_pos(0, 0);
    // Center the button caption
    if (label.length() > 0)
    {
        const auto& font = gui_->skin()->font(Skin::FontStyle::LABEL);
        units::pixel caption_width = font->string_width(label);
        units::pixel letter_height = font->letter_height();
        caption_pos.x = pos.x + floor((pos.w - caption_width) / 2 - font->cursor_offset(label[0]));
        caption_pos.y = pos.y + floor((pos.h + letter_height) / 2);
    }
    label_.reset(new Label(caption_pos, label, parent_manager, parent_window));
}

void Button::Render()
{
    auto layout = gui_->skin()->layout();

    const Skin::Layout::Button* b;
    if (active_ && hover_)
    {
        b = &layout->button.active;
    }
    else if (hover_)
    {
        b = &layout->button.hover;
    }
    else
    {
        b = &layout->button.normal;
    }

    RenderBody(*b);

    // Button text yall
    label_->Render();
}

void Button::RenderBody(const Skin::Layout::Button& b)
{
    Box pos, uv;
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    // Top left corner
    pos.x = x;
    pos.y = y;
    pos.w = b.top_left.w;
    pos.h = b.top_left.h;
    uv = b.top_left;
    gui_->SubmitControlBatch(pos, uv, crop_, feather_);

    // Top edge
    pos.x = x + b.top_left.w;
    pos.y = y;
    pos.w = pos_.w - (b.top_left.w + b.top_right.w);
    pos.h = b.top.h;
    uv = b.top;
    gui_->SubmitControlBatch(pos, uv, crop_, feather_);

    // Top right corner
    pos.x = x + pos_.w - b.top_right.w;
    pos.y = y;
    pos.w = b.top_right.w;
    pos.h = b.top_right.h;
    uv = b.top_right;
    gui_->SubmitControlBatch(pos, uv, crop_, feather_);

    // Left edge
    pos.x = x;
    pos.y = y + b.top_left.h;
    pos.w = b.left.w;
    pos.h = pos_.h - (b.top_left.h + b.bottom_right.h);
    uv = b.left;
    gui_->SubmitControlBatch(pos, uv, crop_, feather_);

    // Body
    pos.x = x + b.left.w;
    pos.y = y + b.top.h;
    pos.w = pos_.w - (b.left.w + b.right.w);
    pos.h = pos_.h - (b.top.h + b.bottom.h);
    uv = b.body;
    gui_->SubmitControlBatch(pos, uv, crop_, feather_);

    // Right edge
    pos.x = x + pos_.w - b.right.w;
    pos.y = y + b.top_right.h;
    pos.w = b.right.w;
    pos.h = pos_.h - (b.top_right.h + b.bottom_right.h);
    uv = b.right;
    gui_->SubmitControlBatch(pos, uv, crop_, feather_);

    // Bottom left corner
    pos.x = x;
    pos.y = y + pos_.h - b.bottom_left.h;
    pos.w = b.bottom_left.w;
    pos.h = b.bottom_left.h;
    uv = b.bottom_left;
    gui_->SubmitControlBatch(pos, uv, crop_, feather_);

    // Bottom edge
    pos.x = x + b.bottom_left.w;
    pos.y = y + pos_.h - b.bottom.h;
    pos.w = pos_.w - (b.bottom_left.w + b.bottom_right.w);
    pos.h = b.bottom.h;
    uv = b.bottom;
    gui_->SubmitControlBatch(pos, uv, crop_, feather_);

    // Bottom right corner
    pos.x = x + pos_.w - b.bottom_right.w;
    pos.y = y + pos_.h - b.bottom_right.h;
    pos.w = b.bottom_right.w;
    pos.h = b.bottom_right.h;
    uv = b.bottom_right;
    gui_->SubmitControlBatch(pos, uv, crop_, feather_);

    // Label base colour
    label_->set_text_colour(b.colour);
}

bool Button::Update(const Input& input)
{
    bool input_handled = false;

    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    units::pixel mx = input.mouse_x();
    units::pixel my = input.mouse_y();

    // Cursor inside button
    if (mx >= x && mx < x + pos_.w &&
        my >= y && my < y + pos_.h)
    {
        hover_ = true;
    }
    else
    {
        hover_ = false;
    }

    for (const auto& e : input.event_queue())
    {
        // Clicked inside button
        if (hover_)
        {
            if (e.type == Input::Event::MOUSE_DOWN)
            {
                active_ = true;
                input_handled = true;
            }
            else if (e.type == Input::Event::MOUSE_UP)
            {
                // Only do callback if cursor is inside button's region when mouse is released
                if (active_)
                {
                    callback_();
                }
                active_ = false;
                input_handled = true;
            }
        }
        else
        {
            if (e.type == Input::Event::MOUSE_UP)
            {
                active_ = false;
            }
        }
    }
    // Swallow input while button is held
    input_handled |= active_;

    return input_handled;
}

void Button::set_callback(std::function<void()> callback)
{
    callback_ = callback;
}
} // namespace gui
} // namespace blons