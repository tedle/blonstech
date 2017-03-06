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
    Box pos, uv;
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;
    units::subpixel pixel_width = static_cast<float>(floor(pos_.w * progress_));

    if (pixel_width > 0.0)
    {
        // Top left corner
        pos.x = x;
        pos.y = y;
        pos.w = std::min(pixel_width, t.top_left.w);
        pos.h = t.top_left.h;
        uv.x = t.top_left.x;
        uv.y = t.top_left.y;
        uv.w = std::min(pixel_width, t.top_left.w);
        uv.h = t.top_left.h;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Left edge
        pos.x = x;
        pos.y = y + t.top_left.h;
        pos.w = std::min(pixel_width, t.left.w);
        pos.h = pos_.h - (t.top_left.h + t.bottom_right.h);
        uv.x = t.left.x;
        uv.y = t.left.y;
        uv.w = std::min(pixel_width, t.left.w);
        uv.h = t.left.h;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Bottom left corner
        pos.x = x;
        pos.y = y + pos_.h - t.bottom_left.h;
        pos.w = std::min(pixel_width, t.bottom_left.w);
        pos.h = t.bottom_left.h;
        uv.x = t.bottom_left.x;
        uv.y = t.bottom_left.y;
        uv.w = std::min(pixel_width, t.bottom_left.w);
        uv.h = t.bottom_left.h;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
    }

    if (pixel_width > t.top_left.w)
    {
        // Top edge
        pos.x = x + t.top_left.w;
        pos.y = y;
        pos.w = std::min(pixel_width - t.top_left.w, pos_.w - (t.top_left.w + t.top_right.w));
        pos.h = t.top.h;
        uv = t.top;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
    }

    if (pixel_width > t.left.w)
    {
        // Body
        pos.x = x + t.left.w;
        pos.y = y + t.top.h;
        pos.w = std::min(pixel_width - t.left.w, pos_.w - (t.left.w + t.right.w));
        pos.h = pos_.h - (t.top.h + t.bottom.h);
        uv = t.body;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
    }

    if (pixel_width > t.bottom_left.w)
    {
        // Bottom edge
        pos.x = x + t.bottom_left.w;
        pos.y = y + pos_.h - t.bottom.h;
        pos.w = std::min(pixel_width - t.bottom_left.w, pos_.w - (t.bottom_left.w + t.bottom_right.w));
        pos.h = t.bottom.h;
        uv = t.bottom;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
    }

    if (pixel_width > pos_.w - t.top_right.w)
    {
        // Top right corner
        pos.x = x + pos_.w - t.top_right.w;
        pos.y = y;
        pos.w = std::min(pixel_width - (pos_.w - t.top_right.w), t.top_right.w);
        pos.h = t.top_right.h;
        uv.x = t.top_right.x;
        uv.y = t.top_right.y;
        uv.w = std::min(pixel_width - (pos_.w - t.top_right.w), t.top_right.w);
        uv.h = t.top_right.h;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
    }

    if (pixel_width > pos_.w - t.right.w)
    {
        // Right edge
        pos.x = x + pos_.w - t.right.w;
        pos.y = y + t.top_right.h;
        pos.w = std::min(pixel_width - (pos_.w - t.right.w), t.right.w);
        pos.h = pos_.h - (t.top_right.h + t.bottom_right.h);
        uv.x = t.right.x;
        uv.y = t.right.y;
        uv.w = std::min(pixel_width - (pos_.w - t.right.w), t.right.w);
        uv.h = t.right.h;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
    }

    if (pixel_width > pos_.w - t.bottom_right.w)
    {
        // Bottom right corner
        pos.x = x + pos_.w - t.bottom_right.w;
        pos.y = y + pos_.h - t.bottom_right.h;
        pos.w = std::min(pixel_width - (pos_.w - t.bottom_right.w), t.bottom_right.w);
        pos.h = t.bottom_right.h;
        uv.x = t.bottom_right.x;
        uv.y = t.bottom_right.y;
        uv.w = std::min(pixel_width - (pos_.w - t.bottom_right.w), t.bottom_right.w);
        uv.h = t.bottom_right.h;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
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