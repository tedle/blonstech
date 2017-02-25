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

#include "debugslidertextbox.h"

// Includes
#include <algorithm>
// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
DebugSliderTextbox::DebugSliderTextbox(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window)
    : Textbox(pos, style, parent_manager, parent_window)
{
    queue_callback_ = false;
    set_progress(0.0f);
}

void DebugSliderTextbox::Render()
{
    auto layout = gui_->skin()->layout();

    const Skin::Layout::Textbox* textbox_layout;
    if (focus())
    {
        textbox_layout = &layout->textbox.active;
    }
    else
    {
        textbox_layout = &layout->textbox.normal;
    }

    RenderBody(*textbox_layout);
    RenderProgress();
    if (focus())
    {
        RenderCursor(layout->textbox.cursor);
    }

    RenderText();
}

void DebugSliderTextbox::RenderProgress()
{
    auto t = gui_->skin()->layout()->textbox.progress;
    auto sprite = gui_->skin()->sprite();
    auto batch = gui_->control_batch(crop_, feather_);
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;
    units::subpixel pixel_width = static_cast<float>(floor(pos_.w * progress_));

    if (pixel_width > 0.0)
    {
        // Top left corner
        sprite->set_pos(x,
                        y,
                        std::min(pixel_width, t.top_left.w),
                        t.top_left.h);
        sprite->set_subtexture(t.top_left.x,
                               t.top_left.y,
                               std::min(pixel_width, t.top_left.w),
                               t.top_left.h);
        batch->Append(sprite->mesh());

        // Left edge
        sprite->set_pos(x,
                        y + t.top_left.h,
                        std::min(pixel_width, t.left.w),
                        pos_.h - (t.top_left.h + t.bottom_right.h));
        sprite->set_subtexture(t.left.x,
                               t.left.y,
                               std::min(pixel_width, t.left.w),
                               t.left.h);
        batch->Append(sprite->mesh());

        // Bottom left corner
        sprite->set_pos(x,
                        y + pos_.h - t.bottom_left.h,
                        std::min(pixel_width, t.bottom_left.w),
                        t.bottom_left.h);
        sprite->set_subtexture(t.bottom_left.x,
                               t.bottom_left.y,
                               std::min(pixel_width, t.bottom_left.w),
                               t.bottom_left.h);
        batch->Append(sprite->mesh());
    }

    if (pixel_width > t.top_left.w)
    {
        // Top edge
        sprite->set_pos(x + t.top_left.w,
                        y,
                        std::min(pixel_width - t.top_left.w, pos_.w - (t.top_left.w + t.top_right.w)),
                        t.top.h);
        sprite->set_subtexture(t.top);
        batch->Append(sprite->mesh());
    }

    if (pixel_width > t.left.w)
    {
        // Body
        sprite->set_pos(x + t.left.w,
                        y + t.top.h,
                        std::min(pixel_width - t.left.w, pos_.w - (t.left.w + t.right.w)),
                        pos_.h - (t.top.h + t.bottom.h));
        sprite->set_subtexture(t.body);
        batch->Append(sprite->mesh());
    }

    if (pixel_width > t.bottom_left.w)
    {
        // Bottom edge
        sprite->set_pos(x + t.bottom_left.w,
                        y + pos_.h - t.bottom.h,
                        std::min(pixel_width - t.bottom_left.w, pos_.w - (t.bottom_left.w + t.bottom_right.w)),
                        t.bottom.h);
        sprite->set_subtexture(t.bottom);
        batch->Append(sprite->mesh());
    }

    if (pixel_width > pos_.w - t.top_right.w)
    {
        // Top right corner
        sprite->set_pos(x + pos_.w - t.top_right.w,
                        y,
                        std::min(pixel_width - (pos_.w - t.top_right.w), t.top_right.w),
                        t.top_right.h);
        sprite->set_subtexture(t.top_right.x,
                               t.top_right.y,
                               std::min(pixel_width - (pos_.w - t.top_right.w), t.top_right.w),
                               t.top_right.h);
        batch->Append(sprite->mesh());
    }

    if (pixel_width > pos_.w - t.right.w)
    {
        // Right edge
        sprite->set_pos(x + pos_.w - t.right.w,
                        y + t.top_right.h,
                        std::min(pixel_width - (pos_.w - t.right.w), t.right.w),
                        pos_.h - (t.top_right.h + t.bottom_right.h));
        sprite->set_subtexture(t.right.x,
                               t.right.y,
                               std::min(pixel_width - (pos_.w - t.right.w), t.right.w),
                               t.right.h);
        batch->Append(sprite->mesh());
    }

    if (pixel_width > pos_.w - t.bottom_right.w)
    {
        // Bottom right corner
        sprite->set_pos(x + pos_.w - t.bottom_right.w,
                        y + pos_.h - t.bottom_right.h,
                        std::min(pixel_width - (pos_.w - t.bottom_right.w), t.bottom_right.w),
                        t.bottom_right.h);
        sprite->set_subtexture(t.bottom_right.x,
                               t.bottom_right.y,
                               std::min(pixel_width - (pos_.w - t.bottom_right.w), t.bottom_right.w),
                               t.bottom_right.h);
        batch->Append(sprite->mesh());
    }
}

bool DebugSliderTextbox::Update(const Input& input)
{
    bool active = Textbox::Update(input);
    // Allows debug slider to sanitize text input when focus is lost
    if (active)
    {
        if (!queue_callback_)
        {
            set_highlight(0, -1);
        }
        queue_callback_ = true;
    }
    else if (queue_callback_)
    {
        queue_callback_ = false;
        callback()(this);
        set_highlight(0, 0);
    }
    return active;
}

void DebugSliderTextbox::set_progress(double progress)
{
    progress_ = std::max(0.0, std::min(1.0, progress));
}
} // namespace gui
} // namespace blons