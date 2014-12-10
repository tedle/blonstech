#include <blons/graphics/gui/skin.h>

// Local Includes
#include <blons/graphics/sprite.h>

namespace blons
{
namespace gui
{
Skin::Skin(RenderContext& context)
{
    skin_ = std::unique_ptr<Sprite>(new Sprite("../../notes/skin.png", context));

    // TODO: Ensure DEFAULT font is somehow not nullptr before being handed over to user
    font_list_[DEFAULT] = nullptr;
    font_list_[HEADING] = nullptr;
    font_list_[LABEL] = nullptr;
    font_list_[CONSOLE] = nullptr;
}

Skin::~Skin()
{

}

bool Skin::LoadFont(std::string filename, FontStyle style, units::pixel pixel_size, RenderContext& context)
{
    font_list_[style] = std::unique_ptr<Font>(new Font(filename, pixel_size, context));
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