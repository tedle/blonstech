#include "graphics/gui/label.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
    Label::Label(int x, int y, const char* text, Manager* parent_manager, Window* parent_window)
{
    pos_ = Box(static_cast<float>(x), static_cast<float>(y), 0, 0);
    text_ = ColourString(text);
    gui_ = parent_manager;
    parent_ = parent_window;
}

void Label::Render(RenderContext& context)
{
    auto font = gui_->skin()->font(FontType::LABEL);

    auto parent_pos = parent_->pos();
    int x = static_cast<int>(pos_.x + parent_pos.x);
    int y = static_cast<int>(pos_.y + parent_pos.y);
    for (const auto& frag : text_.fragments())
    {
        // One draw call per (colour,font) used across all labels combined
        auto batcher = font_batch(FontType::LABEL, frag.colour, context);
        for (const auto& c : frag.text)
        {
            batcher->Append(*font->BuildSprite(c, x, y)->mesh());
            x += font->advance();
        }
    }

    RegisterBatches();
}

bool Label::Update(const Input& input)
{
    return false;
}
} // namespace GUI
} // namespace blons