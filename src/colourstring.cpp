#include "graphics/gui/colourstring.h"

namespace blons
{
namespace GUI
{
ColourString::ColourString()
{
}

ColourString::ColourString(std::string text)
{
    // split in 2 and colour each piece
    std::size_t len = text.length();
    if (len > 2)
    {
        std::size_t half_len = len / 2;
        Fragment f1 = { Vector4(1.0, 1.0, 1.0, 1.0), text.substr(0, half_len) };
        Fragment f2 = { Vector4(0.0, 0.0, 0.0, 1.0), text.substr(half_len) };
        text_fragments_.push_back(f1);
        text_fragments_.push_back(f2);
    }
    else
    {
        Fragment f = { Vector4(1.0, 1.0, 1.0, 1.0), text };
        text_fragments_.push_back(f);
    }
}

ColourString::~ColourString()
{
}

const std::vector<ColourString::Fragment>& ColourString::fragments()
{
    return text_fragments_;
}
} // namespace GUI
} // namespace blons