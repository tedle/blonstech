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
        // Since crop boxes are volatile we don't wanna overpopulate batch cache
        // Cropping info is injected into a new copy here
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

DrawBatcher* Control::font_batch(FontStyle style, Vector4 colour, RenderContext& context)
{
    StaticDrawCallInputs inputs = { true, style, colour };
    return batch(inputs, context);
}

DrawBatcher* Control::control_batch(RenderContext& context)
{
    StaticDrawCallInputs inputs = { false, FontStyle::DEFAULT, Vector4(0, 0, 0, 0) };
    return batch(inputs, context);
}
} // namespace gui
} // namespace blons