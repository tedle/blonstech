#ifndef BLONSTECH_GRAPHICS_GUI_FONT_H_
#define BLONSTECH_GRAPHICS_GUI_FONT_H_

// Includes
#include <vector>
// Local Includes
#include "graphics/sprite.h"

namespace blons
{
namespace GUI
{
class Font
{
public:
    Font(const char* font_filename, int pixel_size, RenderContext& context);
    ~Font();

    const std::string kAvailableCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                             "abcdefghijklmnopqrstuvwxyz"
                                             "0123456789!@#$%^&*()"
                                             " ,./<>?;':\"[]\\{}|-=_+";

    Sprite* BuildSprite(unsigned char letter, int x, int y);

    // Return how far to advance horizontally after rendering a character in pixels
    // Resets to 0 after call
    int advance();
    int index_count();
    TextureResource* texture();

private:
    struct Glyph;
    std::vector<Glyph> charset_;
    std::unique_ptr<class Sprite> fontsheet_;
    std::size_t pixel_size_;
    int advance_;
};
} // namespace GUI
} // namespace blons
#endif