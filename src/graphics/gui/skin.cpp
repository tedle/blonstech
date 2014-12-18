#include <blons/graphics/gui/skin.h>

// Public Includes
#include <blons/graphics/sprite.h>

namespace blons
{
namespace gui
{
Skin::Skin(RenderContext& context)
{
    // TODO: Load from user supplied image
    skin_.reset(new Sprite("../../notes/skin.png", context));

    // TODO: Ensure DEFAULT font is somehow not nullptr before being handed over to user
    font_list_[DEFAULT] = nullptr;
    font_list_[HEADING] = nullptr;
    font_list_[LABEL] = nullptr;
    font_list_[CONSOLE] = nullptr;
}

bool Skin::LoadFont(std::string filename, units::pixel pixel_size, FontStyle style, RenderContext& context)
{
    font_list_[style].reset(new Font(filename, pixel_size, context));
    return true;
}

Font* Skin::font(FontStyle style)
{
    auto& font = font_list_[style];
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
} // namespace gui
} // namespace blons