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
    Sprite* test();
    const std::string kAvailableCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                             "abcdefghijklmnopqrstuvwxyz"
                                             "0123456789!@#$%^&*()"
                                             " ,./<>?;':\"[]\\{}|-=_+";

private:
    struct Glyph;
    std::map<unsigned char, Glyph> charset_;
    std::unique_ptr<Sprite> fontsheet_;
    std::size_t pixel_size_;
};
} // namespace blons
#endif