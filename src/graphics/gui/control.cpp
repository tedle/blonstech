////////////////////////////////////////////////////////////////////////////////
/// blonstech
/// Copyright(c) 2014 Dominic Bowden
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files(the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/graphics/gui/control.h>

// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
void Control::hide()
{
    hidden_ = true;
}

void Control::show()
{
    hidden_ = false;
}

bool Control::hidden()
{
    return hidden_;
}

void Control::set_pos(units::subpixel x, units::subpixel y)
{
    pos_.x = x;
    pos_.y = y;
}

Box Control::pos() const
{
    return pos_;
}

void Control::set_crop(Box crop, units::pixel feather)
{
    crop_ = crop;
    feather_ = feather;
}

void Control::RegisterBatches()
{
    for (std::size_t i = 0; i < batch_index_; i++)
    {
        const auto& batch = draw_batches_[i];
        // Since crop boxes are volatile and we don't wanna overpopulate the batch cache,
        // cropping info is injected into a new copy here
        DrawCallInputs batch_info =
        {
            batch.first.is_text,
            batch.first.font_style,
            batch.first.colour,
            crop_,
            feather_
        };
        gui_->RegisterDrawCall(batch_info, batch.second.get());
    }
    batch_index_ = 0;
}

void Control::ClearBatches()
{
    draw_batches_.clear();
    batch_index_ = 0;
}

// This is setup to recycle memory as much as we can. Used like a C array
// that is only resized when the total batches for a frame is greater
// than any previous frame. Can only be reset manually by calling ClearBatches().
DrawBatcher* Control::batch(StaticDrawCallInputs inputs, RenderContext& context)
{
    DrawBatcher* batch;
    if (batch_index_ < draw_batches_.size())
    {
        draw_batches_[batch_index_].first = inputs;
        batch = draw_batches_[batch_index_].second.get();
    }
    else
    {
        auto batcher = std::unique_ptr<DrawBatcher>(new DrawBatcher(context));
        draw_batches_.push_back(std::make_pair(inputs, std::move(batcher)));
        batch = draw_batches_.back().second.get();
    }

    batch_index_++;
    return batch;
}

DrawBatcher* Control::font_batch(Skin::FontStyle style, Vector4 colour, RenderContext& context)
{
    StaticDrawCallInputs inputs = { true, style, colour };
    return batch(inputs, context);
}

DrawBatcher* Control::control_batch(RenderContext& context)
{
    StaticDrawCallInputs inputs = { false, Skin::FontStyle::DEFAULT, Vector4(0, 0, 0, 0) };
    return batch(inputs, context);
}
} // namespace gui
} // namespace blons