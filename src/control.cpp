#include "graphics/gui/control.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
void Control::set_pos(float x, float y)
{
    pos_.x = x;
    pos_.y = y;
}

Box Control::pos() const
{
    return pos_;
}

void Control::set_crop(Box crop, int feather)
{
    crop_ = crop;
    feather_ = feather;
}

void Control::RegisterBatches()
{
    for (const auto& batch : draw_batches_)
    {
        // Since crop boxes are volatile we don't wanna overpopulate batch cache
        // Cropping info is injected into a new copy here
        DrawCallInfo batch_info =
        {
            batch.first.is_text,
            batch.first.usage,
            batch.first.colour,
            crop_,
            feather_
        };
        gui_->RegisterDrawCall(batch_info, batch.second.get());
    }
}

void Control::ClearBatches()
{
    draw_batches_.clear();
}

// TODO: Make font_batch + control_batch more DRY
DrawBatcher* Control::font_batch(FontType usage, Vector4 colour, RenderContext& context)
{
    StaticDrawCallInfo call = { true, usage, colour };
    auto index_match = draw_batches_.find(call);
    if (index_match != draw_batches_.end())
    {
        return index_match->second.get();
    }

    auto batcher = std::unique_ptr<DrawBatcher>(new DrawBatcher(context));
    draw_batches_.insert(draw_batches_.begin(),
                         std::make_pair(call, std::move(batcher)));
    return draw_batches_.begin()->second.get();
}

DrawBatcher* Control::control_batch(RenderContext& context)
{
    StaticDrawCallInfo call = { false, FontType::DEFAULT, Vector4(0, 0, 0, 0) };
    auto index_match = draw_batches_.find(call);
    if (index_match != draw_batches_.end())
    {
        return index_match->second.get();
    }

    auto batcher = std::unique_ptr<DrawBatcher>(new DrawBatcher(context));
    draw_batches_.insert(draw_batches_.begin(),
                         std::make_pair(call, std::move(batcher)));
    return draw_batches_.begin()->second.get();
}
} // namespace GUI
} // namespace blons