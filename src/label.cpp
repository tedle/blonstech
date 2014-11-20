#include "graphics/gui/label.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
Label::Label(int x, int y, const char* text, Manager* parent_manager)
{
    pos_ = Box(static_cast<float>(x), static_cast<float>(y), 0, 0);
    text_ = ColourString(text);
    gui_ = parent_manager;
}

void Label::Render(RenderContext& context)
{
    auto font = gui_->GetFont(FontType::LABEL);

    int x = static_cast<int>(pos_.x);
    int y = static_cast<int>(pos_.y);
    for (const auto& frag : text_.fragments())
    {
        // One draw call per (colour,font) used across all labels combined
        auto batcher = gui_->GetFontBatch(FontType::LABEL, frag.colour, context);
        for (const auto& c : frag.text)
        {
            batcher->Append(*font->BuildSprite(c, x, y)->mesh());
            x += font->advance();
        }
    }
}
} // namespace GUI
} // namespace blons