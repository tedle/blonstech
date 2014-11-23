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
        struct WindowLayout
        {
            struct TitleBarLayout
            {
                Box left = Box(0, 0, 3, 5);
                Box center = Box(4, 0, 3, 5);
                Box right = Box(8, 0, 3, 5);
            } title;
            Box top_left = Box(0, 6, 5, 5);
            Box top = Box(6, 6, 3, 5);
            Box top_right = Box(10, 6, 5, 5);
            Box left = Box(0, 12, 5, 3);
            Box body = Box(6, 12, 3, 3);
            Box right = Box(10, 12, 5, 3);
            Box bottom_left = Box(0, 16, 5, 5);
            Box bottom = Box(6, 16, 3, 5);
            Box bottom_right = Box(10, 16, 5, 5);
        } window;
        Box button = Box(0, 0, 100, 100);
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