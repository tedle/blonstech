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
        struct ButtonSetLayout
        {
            struct ButtonLayout
            {
                Box top_left;
                Box top;
                Box top_right;
                Box left;
                Box body;
                Box right;
                Box bottom_left;
                Box bottom;
                Box bottom_right;
            };
            // Wish initializers didnt suck so much...
            ButtonLayout normal = ButtonLayout 
            {
                Box(0, 45, 5, 5), // top_left
                Box(6, 45, 1, 5), // top
                Box(8, 45, 5, 5), // top_right
                Box(0, 51, 5, 1), // left
                Box(6, 51, 1, 1), // body
                Box(8, 51, 5, 1), // right
                Box(0, 53, 5, 5), // bottom_left
                Box(6, 53, 1, 5), // bottom
                Box(8, 53, 5, 5)  // bottom_right
            };
            ButtonLayout hover = ButtonLayout
            {
                Box(0, 59, 5, 5), // top_left
                Box(6, 59, 1, 5), // top
                Box(8, 59, 5, 5), // top_right
                Box(0, 65, 5, 1), // left
                Box(6, 65, 1, 1), // body
                Box(8, 65, 5, 1), // right
                Box(0, 67, 5, 5), // bottom_left
                Box(6, 67, 1, 5), // bottom
                Box(8, 67, 5, 5)  // bottom_right
            };
            ButtonLayout active = ButtonLayout
            {
                Box(0, 73, 5, 5), // top_left
                Box(6, 73, 1, 5), // top
                Box(8, 73, 5, 5), // top_right
                Box(0, 79, 5, 1), // left
                Box(6, 79, 1, 1), // body
                Box(8, 79, 5, 1), // right
                Box(0, 81, 5, 5), // bottom_left
                Box(6, 81, 1, 5), // bottom
                Box(8, 81, 5, 5)  // bottom_right
            };
        } button;

        struct TextboxSetLayout
        {
            struct TextboxLayout
            {
                Box top_left;
                Box top;
                Box top_right;
                Box left;
                Box body;
                Box right;
                Box bottom_left;
                Box bottom;
                Box bottom_right;
            };
            TextboxLayout normal = TextboxLayout
            {
                Box(14, 0, 5, 5), // top_left
                Box(20, 0, 1, 5), // top
                Box(22, 0, 5, 5), // top_right
                Box(14, 6, 5, 1), // left
                Box(20, 6, 1, 1), // body
                Box(22, 6, 5, 1), // right
                Box(14, 8, 5, 5), // bottom_left
                Box(20, 8, 1, 5), // bottom
                Box(22, 8, 5, 5)  // bottom_right
            };
            TextboxLayout active = TextboxLayout
            {
                Box(14, 14, 5, 5), // top_left
                Box(20, 14, 1, 5), // top
                Box(22, 14, 5, 5), // top_right
                Box(14, 20, 5, 1), // left
                Box(20, 20, 1, 1), // body
                Box(22, 20, 5, 1), // right
                Box(14, 22, 5, 5), // bottom_left
                Box(20, 22, 1, 5), // bottom
                Box(22, 22, 5, 5)  // bottom_right
            };
        } textbox;

        struct WindowLayout
        {
            struct TitleBarLayout
            {
                Box left = Box(0, 0, 5, 30);
                Box center = Box(6, 0, 1, 30);
                Box right = Box(8, 0, 5, 30);
            } title;
            Box top_left = Box(0, 31, 5, 5);
            Box top = Box(6, 31, 1, 5);
            Box top_right = Box(8, 31, 5, 5);
            Box left = Box(0, 37, 5, 1);
            Box body = Box(6, 37, 1, 1);
            Box right = Box(8, 37, 5, 1);
            Box bottom_left = Box(0, 39, 5, 5);
            Box bottom = Box(6, 39, 1, 5);
            Box bottom_right = Box(8, 39, 5, 5);
        } window;
    };

public:
    Skin(RenderContext& context);
    ~Skin();

    bool LoadFont(std::string filename, FontType usage, int pixel_size, RenderContext& context);
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