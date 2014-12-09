#ifndef BLONSTECH_GRAPHICS_GUI_COLOURSTRING_H_
#define BLONSTECH_GRAPHICS_GUI_COLOURSTRING_H_

// Includes
#include <vector>
// Local Includes
#include "math/math.h"

namespace blons
{
namespace GUI
{
// White
const Vector4 kDefaultTextColour = Vector4(1.0, 1.0, 1.0, 1.0);
// Formatting:
//     $fff = hex colour code
//         "white text$f00red text$0f0green text!"
class ColourString
{
public:
    // Store text as <RED,"hello ">,<BLUE,"world!">,etc
    struct Fragment
    {
        Vector4 colour;
        std::string text;
    };

public:
    ColourString(std::string text, Vector4 base_colour);
    ColourString(std::string text) : ColourString(text, kDefaultTextColour) {}
    ColourString() : ColourString("") {}
    ~ColourString() {}

    static std::string MakeColourCode(Vector4 colour);

    const Vector4& base_colour() const;
    const std::vector<Fragment>& fragments() const;
    const std::string& str() const;
    // Unparsed colour string
    const std::string& raw_str() const;

private:
    Vector4 base_colour_;
    std::vector<Fragment> text_fragments_;
    std::string text_;
    std::string raw_text_;
};
} // namespace GUI
} // namespace blons
#endif