#ifndef BLONSTECH_FONT_H_
#define BLONSTECH_FONT_H_

// Includes
#include <map>
#include <vector>
// Local Includes
#include "sprite.h"

namespace blons
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

    bool Render(unsigned char letter, int x, int y, RenderContext& context);

    // Return how far to advance horizontally after rendering a character in pixels
    // Resets to 0 after call
    int advance();
    int index_count();
    TextureResource* texture();

private:
    struct Glyph;
    std::map<unsigned char, Glyph> charset_;
    std::unique_ptr<Sprite> fontsheet_;
    std::size_t pixel_size_;
    int advance_;
};
} // namespace blons
#endif