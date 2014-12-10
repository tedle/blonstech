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
namespace gui
{
enum FontStyle
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
        struct Button
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
            Vector4 colour;
        };

        struct ButtonSet
        {
            // Wish initializers didnt suck so much...
            Button normal = Button
            {
                Box(0, 45, 5, 5), // top_left
                Box(6, 45, 1, 5), // top
                Box(8, 45, 5, 5), // top_right
                Box(0, 51, 5, 1), // left
                Box(6, 51, 1, 1), // body
                Box(8, 51, 5, 1), // right
                Box(0, 53, 5, 5), // bottom_left
                Box(6, 53, 1, 5), // bottom
                Box(8, 53, 5, 5), // bottom_right
                Vector4(1.0, 1.0, 1.0, 1.0)
            };
            Button hover = Button
            {
                Box(0, 59, 5, 5), // top_left
                Box(6, 59, 1, 5), // top
                Box(8, 59, 5, 5), // top_right
                Box(0, 65, 5, 1), // left
                Box(6, 65, 1, 1), // body
                Box(8, 65, 5, 1), // right
                Box(0, 67, 5, 5), // bottom_left
                Box(6, 67, 1, 5), // bottom
                Box(8, 67, 5, 5), // bottom_right
                Vector4(1.0, 1.0, 1.0, 1.0)
            };
            Button active = Button
            {
                Box(0, 73, 5, 5), // top_left
                Box(6, 73, 1, 5), // top
                Box(8, 73, 5, 5), // top_right
                Box(0, 79, 5, 1), // left
                Box(6, 79, 1, 1), // body
                Box(8, 79, 5, 1), // right
                Box(0, 81, 5, 5), // bottom_left
                Box(6, 81, 1, 5), // bottom
                Box(8, 81, 5, 5), // bottom_right
                Vector4(1.0, 1.0, 1.0, 1.0)
            };
        } button;

        struct Textarea
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
            Vector4 colour;
        };

        Textarea textarea = Textarea
        {
            Box(14, 28, 5, 5), // top_left
            Box(20, 28, 1, 5), // top
            Box(22, 28, 5, 5), // top_right
            Box(14, 34, 5, 1), // left
            Box(20, 34, 1, 1), // body
            Box(22, 34, 5, 1), // right
            Box(14, 36, 5, 5), // bottom_left
            Box(20, 36, 1, 5), // bottom
            Box(22, 36, 5, 5), // bottom_right
            Vector4(1.0, 1.0, 1.0, 1.0)
        };

        struct Textbox
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
            Vector4 colour;
        };

        struct TextboxSet
        {
            Textbox normal = Textbox
            {
                Box(14, 0, 5, 5), // top_left
                Box(20, 0, 1, 5), // top
                Box(22, 0, 5, 5), // top_right
                Box(14, 6, 5, 1), // left
                Box(20, 6, 1, 1), // body
                Box(22, 6, 5, 1), // right
                Box(14, 8, 5, 5), // bottom_left
                Box(20, 8, 1, 5), // bottom
                Box(22, 8, 5, 5), // bottom_right
                Vector4(0.8f, 0.8f, 0.8f, 1.0)
            };
            Textbox active = Textbox
            {
                Box(14, 14, 5, 5), // top_left
                Box(20, 14, 1, 5), // top
                Box(22, 14, 5, 5), // top_right
                Box(14, 20, 5, 1), // left
                Box(20, 20, 1, 1), // body
                Box(22, 20, 5, 1), // right
                Box(14, 22, 5, 5), // bottom_left
                Box(20, 22, 1, 5), // bottom
                Box(22, 22, 5, 5), // bottom_right
                Vector4(1.0, 1.0, 1.0, 1.0)
            };
            Box cursor = Box(14, 42, 1, 1);
        } textbox;

        struct Window
        {
            struct Titlebar
            {
                Box left = Box(0, 0, 5, 30);
                Box center = Box(6, 0, 1, 30);
                Box right = Box(8, 0, 5, 30);
                Vector4 colour = Vector4(0.25f, 0.25f, 0.25f, 1.0);
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
            Vector4 colour = Vector4(1.0, 0.0, 1.0, 1.0);
        } window;

        struct Console
        {
            Textarea textarea = Textarea
            {
                Box(56, 0, 10, 10),  // top_left
                Box(67, 0, 1, 10),   // top
                Box(69, 0, 10, 10),  // top_right
                Box(56, 11, 10, 23), // left
                Box(67, 11, 1, 23),  // body
                Box(69, 11, 10, 23), // right
                Box(56, 35, 10, 10), // bottom_left
                Box(67, 35, 1, 10),  // bottom
                Box(69, 35, 10, 10), // bottom_right
                Vector4(0.9f, 0.9f, 0.9f, 1.0)
            };
            Textbox textbox = Textbox
            {
                Box(30, 0, 10, 10),  // top_left
                Box(41, 0, 1, 10),   // top
                Box(43, 0, 10, 10),  // top_right
                Box(30, 11, 10, 1),  // left
                Box(41, 11, 1, 1),   // body
                Box(43, 11, 10, 1),  // right
                Box(30, 13, 10, 10), // bottom_left
                Box(41, 13, 1, 10),  // bottom
                Box(43, 13, 10, 10), // bottom_right
                Vector4(0.9f, 0.9f, 0.9f, 1.0)
            };
            Box cursor = Box(56, 48, 1, 1);
        } console;

        struct Dropshadow
        {
            Box bottom_left = Box(82, 0, 10, 40);
            Box bottom = Box(93, 0, 1, 40);
            Box bottom_right = Box(95, 0, 10, 40);
        } dropshadow;
    };

public:
    Skin(RenderContext& context);
    ~Skin();

    bool LoadFont(std::string filename, FontStyle style, units::pixel pixel_size, RenderContext& context);
    Font* font(FontStyle style);
    Sprite* sprite() const;
    const Layout* layout() const;

protected:
    Layout layout_;
    std::unique_ptr<Sprite> skin_;
    std::map<FontStyle, std::unique_ptr<Font>> font_list_;

};
} // namespace gui
} // namespace blons
#endif