#include "graphics/gui/skin.h"

// Local Includes
#include "graphics/sprite.h"

namespace blons
{
namespace GUI
{
Skin::Skin(RenderContext& context)
{
    skin_ = std::unique_ptr<Sprite>(new Sprite("../../notes/skin.png", context));

    font_list_[DEFAULT] = nullptr;
    font_list_[HEADING] = nullptr;
    font_list_[LABEL] = nullptr;
    font_list_[CONSOLE] = nullptr;
}

Skin::~Skin()
{

}

bool Skin::LoadFont(const char* filename, FontType usage, int pixel_size, RenderContext& context)
{
    font_list_[usage] = std::unique_ptr<Font>(new Font(filename, pixel_size, context));
    return true;
}

Font* Skin::font(FontType usage)
{
    auto& font = font_list_[usage];
    if (font != nullptr)
    {
        return font.get();
    }
    return font_list_[DEFAULT].get();
}

Sprite* Skin::sprite() const
{
    return skin_.get();
}

const Skin::Layout* Skin::layout() const
{
    return &layout_;
}
} // namespace GUI
} // namespace blons