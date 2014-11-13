#include "graphics/gui/label.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
Label::Label(int x, int y, const char* text, Manager* parent_manager)
{
    pos_ = Vector2(static_cast<float>(x), static_cast<float>(y));
    text_ = text;
    gui_ = parent_manager;
}

Label::~Label()
{
}

void Label::Render(RenderContext& context)
{
    auto batcher = gui_->GetFontBatch(FontType::LABEL, Vector4(1.0, 1.0, 1.0, 1.0), context);
    auto font = gui_->GetFont(FontType::LABEL);

    int x = static_cast<int>(pos_.x);
    int y = static_cast<int>(pos_.y);
    for (const auto& c : text_)
    {
        batcher->Append(*font->BuildSprite(c, x, y)->mesh());
        x += font->advance();
    }
}
} // namespace GUI
} // namespace blons