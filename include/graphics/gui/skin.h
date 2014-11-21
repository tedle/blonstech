#ifndef BLONSTECH_GRAPHICS_GUI_SKIN_H_
#define BLONSTECH_GRAPHICS_GUI_SKIN_H_

// Includes
#include <map>
#include <memory>
// Local Includes
#include "graphics/gui/font.h"
#include "graphics/render/render.h"

namespace blons
{
// Forward declarations
class Sprite;
namespace GUI
{
enum FontType
{
    DEFAULT,
    HEADING,
    LABEL,
    CONSOLE
};
class Skin
{
public:
    // Defined in the header file for all to see
    struct Layout
    {
        Box button = Box(0, 0, 100, 100);
        Box window = Box(0, 200, 100, 100);
    };

public:
    Skin(RenderContext& context);
    ~Skin();

    bool LoadFont(const char* filename, FontType usage, int pixel_size, RenderContext& context);
    Font* font(FontType usage);
    Sprite* sprite() const;
    const Layout* layout() const;

protected:
    Layout layout_;
    std::unique_ptr<Sprite> skin_;
    std::map<FontType, std::unique_ptr<Font>> font_list_;

};
} // namespace GUI
} // namespace blons
#endif