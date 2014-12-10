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

int IntToHex(unsigned char c)
{
    if (c >= 0xA && c <= 0xF)
    {
        // 'A' is charcode 65, 10 is value of A in hex
        return c + 65 - 10;
    }
    else if (c >= 0 && c <= 9)
    {
        return c + 48;
    }
    throw "This shouldn't happen...";
}
} // namespace

namespace blons
{
namespace gui
{
ColourString::ColourString(std::string text, Vector4 base_colour)
{
    raw_text_ = text;
    base_colour_ = base_colour;

    Fragment next_frag;
    next_frag.colour = base_colour_;
    next_frag.is_base = true;
    while (text.length() > 0)
    {
        std::size_t code_pos = FindColourCode(text.c_str());
        next_frag.text = text.substr(0, code_pos);
        if (next_frag.text.length() > 0)
        {
            text_ += next_frag.text;
            text_fragments_.push_back(next_frag);
        }
        if (code_pos != std::string::npos)
        {
            Vector4 colour;
            // Prefix ++ to skip over '$' char
            colour.x = HexToInt(text[++code_pos]) / 15.0f;
            colour.y = HexToInt(text[++code_pos]) / 15.0f;
            colour.z = HexToInt(text[++code_pos]) / 15.0f;
            colour.w = 1.0;
            text = text.substr(++code_pos);
            next_frag.colour = colour;
            next_frag.is_base = false;
        }
        else
        {
            text.clear();
        }
    }
}

std::string ColourString::MakeColourCode(Vector4 colour)
{
    // xxx to avoid extra allocation
    std::string ret = "$xxx";
    ret[1] = IntToHex(static_cast<unsigned char>(15.0f * colour.x));
    ret[2] = IntToHex(static_cast<unsigned char>(15.0f * colour.y));
    ret[3] = IntToHex(static_cast<unsigned char>(15.0f * colour.z));
    return ret;
}

const Vector4& ColourString::base_colour() const
{
    return base_colour_;
}

void ColourString::set_base_colour(Vector4 colour)
{
    base_colour_ = colour;
    for (auto& frag : text_fragments_)
    {
        if (frag.is_base)
        {
            frag.colour = base_colour_;
        }
    }
}

const std::vector<ColourString::Fragment>& ColourString::fragments() const
{
    return text_fragments_;
}

const std::string& ColourString::str() const
{
    return text_;
}

const std::string& ColourString::raw_str() const
{
    return raw_text_;
}
} // namespace gui
} // namespace blons