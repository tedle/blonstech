#ifndef BLONSTECH_GRAPHICS_GUI_FONT_H_
#define BLONSTECH_GRAPHICS_GUI_FONT_H_

// Includes
#include <vector>
// Public Includes
#include <blons/math/units.h>
#include <blons/graphics/sprite.h>
#include <blons/graphics/gui/colourstring.h>

namespace blons
{
namespace gui
{
class Font
{
public:
    Font(std::string font_filename, units::pixel pixel_size, RenderContext& context);
    ~Font();

    const std::string kAvailableCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                             "abcdefghijklmnopqrstuvwxyz"
                                             "`~0123456789!@#$%^&*()"
                                             " ,./<>?;':\"[]\\{}|-=_+";

    Sprite* BuildSprite(unsigned char letter, units::subpixel x, units::subpixel y);
    // Returns nullptr for completely cropped sprites
    Sprite* BuildSprite(unsigned char letter, units::subpixel x, units::subpixel y, Box crop);

    units::pixel cursor_offset(unsigned char letter) const;
    // TODO: Might need to make this const char* for perf later, if its used a lot
    // Defaults trim_whitespace to true
    units::pixel string_width(std::string string) const;
    units::pixel string_width(std::string string, bool trim_whitespace) const;
    // Breaks string into an array of strings guranteed to each be shorter than max_width
    std::vector<std::string> string_wrap(std::string string, units::pixel max_width);
    // Same as above except colour codes don't count towards max_width
    // Also colour codes are insert on newline to ensure they are carried over
    std::vector<std::string> string_wrap(ColourString string, units::pixel max_width);
    // Return how far to advance horizontally after rendering a character in pixels
    // Resets to 0 after call
    units::pixel advance();
    unsigned int index_count() const;
    units::pixel letter_height() const;
    units::pixel line_height() const;
    units::pixel pixel_size() const;
    const TextureResource* texture() const;

private:
    struct Glyph;
    std::vector<Glyph> charset_;
    std::unique_ptr<class Sprite> fontsheet_;
    units::pixel pixel_size_;
    // Max letter height of chars A-Z, used for offsetting or centering text
    units::pixel letter_height_;
    // How much to advance y on newline
    units::pixel line_height_;
    units::pixel advance_;
};
} // namespace gui
} // namespace blons

#endif // BLONSTECH_GRAPHICS_GUI_FONT_H_