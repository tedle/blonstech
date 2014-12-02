#include "graphics/gui/colourstring.h"

// Helper functions
namespace
{
std::size_t FindColourCode(const char* text)
{
    // Simple parser to find '$fff' hex colour codes in strings
    static const std::string kFormatChars = "abcdefABCDEF0123456789";
    std::size_t code_pos = std::string::npos;
    int format_matches = 0;
    for (std::size_t i = 0; text[i] != '\0'; i++)
    {
        if (text[i] == '$')
        {
            code_pos = i;
            format_matches = 1;
        }
        else if (kFormatChars.find(text[i]) != std::string::npos && format_matches >= 1)
        {
            format_matches++;
        }
        else
        {
            code_pos = std::string::npos;
            format_matches = 0;
        }

        if (format_matches == 4)
        {
            return code_pos;
        }
    }
    return std::string::npos;
}

int HexToInt(unsigned char c)
{
    if (c >= 'A' && c <= 'F')
    {
        // 'A' is charcode 65, 10 is value of A in hex
        return c - 65 + 10;
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c - 97 + 10;
    }
    else if (c >= '0' && c <= '9')
    {
        return c - 48;
    }
    throw "This shouldn't happen...";
}
} // namespace

namespace blons
{
namespace GUI
{
ColourString::ColourString()
{
}

ColourString::ColourString(std::string text)
{
    Fragment next_frag;
    next_frag.colour = kDefaultTextColour;
    while (text.length() > 0)
    {
        std::size_t code_pos = FindColourCode(text.c_str());
        next_frag.text = text.substr(0, code_pos);
        if (next_frag.text.length() > 0)
        {
            text_fragments_.push_back(next_frag);
        }
        if (code_pos != std::string::npos)
        {
            Vector4 colour;
            // Prefix ++ to skip over '$' char
            colour.x = HexToInt(text[++code_pos]) / 16.0f;
            colour.y = HexToInt(text[++code_pos]) / 16.0f;
            colour.z = HexToInt(text[++code_pos]) / 16.0f;
            colour.w = 1.0;
            text = text.substr(++code_pos);
            next_frag.colour = colour;
        }
        else
        {
            text.clear();
        }
    }
}

ColourString::~ColourString()
{
}

const std::vector<ColourString::Fragment>& ColourString::fragments() const
{
    return text_fragments_;
}
} // namespace GUI
} // namespace blons