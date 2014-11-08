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
    Font(const char* font_filename, RenderContext& context);
    ~Font();
    Sprite* test();

private:
    class Glyph;
    std::map<unsigned char, Glyph> charset_;
    std::unique_ptr<Sprite> fontsheet_;
};
} // namespace blons
#endif