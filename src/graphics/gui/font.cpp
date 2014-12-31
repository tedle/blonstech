////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/graphics/gui/font.h>

// Includes
#include <algorithm>
#include <FreeType2/include/ft2build.h>
#include FT_FREETYPE_H
// Public Includes
#include <blons/graphics/render/render.h>

namespace blons
{
namespace gui
{
struct Font::Glyph
{
    // Needed for vector.resize()
    Glyph() : width(0), height(0), tex_offset(0), x_offset(0), y_offset(0), x_advance(0) {}
    // This constructor's really just for refactoring code into smaller functions
    Glyph(unsigned char letter, FT_Face font_face, units::pixel texture_offset);
    // 8-bit monochrome bitmap of character data
    std::vector<unsigned char> pixels;
    // Width and height of bitmap
    units::pixel width, height;
    // Offset from origin in fontsheet texture
    units::pixel tex_offset;
    // Offset from cursor,line respectively
    units::pixel x_offset, y_offset;
    // Offset from previously rendered character
    units::pixel x_advance;
};

Font::Glyph::Glyph(unsigned char letter, FT_Face font_face, units::pixel texture_offset)
{
    unsigned int glyph_index = FT_Get_Char_Index(font_face, letter);
    if (!glyph_index)
    {
        throw "Couldn't locate character";
    }

    if (FT_Load_Glyph(font_face, glyph_index, FT_LOAD_DEFAULT) != 0)
    {
        throw "Couldn't load character";
    }
    if (FT_Render_Glyph(font_face->glyph, FT_RENDER_MODE_NORMAL) != 0)
    {
        throw "Couldn't render character";
    }
    if (font_face->glyph->format != FT_GLYPH_FORMAT_BITMAP ||
        font_face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
    {
        throw "Incorrent format of character";
    }

    FT_Bitmap bitmap = font_face->glyph->bitmap;
    width = bitmap.width;
    height = bitmap.rows;
    tex_offset = texture_offset;
    x_advance = font_face->glyph->advance.x / 64;
    x_offset = font_face->glyph->metrics.horiBearingX / 64;
    y_offset = font_face->glyph->metrics.horiBearingY / 64 - height;

    for (units::pixel y = 0; y < bitmap.rows; y++)
    {
        for (units::pixel x = 0; x < bitmap.width; x++)
        {
            pixels.push_back(*(bitmap.buffer++));
        }
        // Accounts for bitmap padding
        bitmap.buffer += (bitmap.pitch - bitmap.width);
    }
}

Font::Font(std::string font_filename, units::pixel pixel_size, RenderContext& context)
{
    fontsheet_ = nullptr;
    letter_height_ = 0;
    pixel_size_ = pixel_size;
    // std::map would be a lot cleaner than a vector, but we need very fast access :)
    size_t largest_char = *std::max_element(kAvailableCharacters.begin(),
                                            kAvailableCharacters.end()) + 1;
    charset_.resize(largest_char);

    FT_Library library;
    FT_Face face;

    if (FT_Init_FreeType(&library) != 0)
    {
        throw "Couldn't initialize font library";
    }

    if (FT_New_Face(library, font_filename.c_str(), 0, &face) != 0)
    {
        throw "Couldn't load font file";
    }

    if (FT_Set_Pixel_Sizes(face, 0, pixel_size_) != 0)
    {
        throw "Couldn't set pixel size";
    }

    line_height_ = face->size->metrics.height / 64;

    // Used to find glyph in texture fontsheet later
    units::pixel tex_width = 0;
    // Used to determine how tall font texture must be
    units::pixel tex_height = 0;
    // For every drawable character, get a bitmap rendering of the letter and store it
    for (const auto& c : kAvailableCharacters)
    {
        Glyph g(c, face, tex_width);
        charset_[c] = g;
        tex_width += g.width;
        // Funky parentheses to appease the lord of windows macros
        tex_height = (std::max)(g.height, tex_height);
        if (c >= 'A' && c <= 'Z' && c != 'J' && c != 'Q')
        {
            letter_height_ = (std::max)(g.height, letter_height_);
        }
    }

    // Generate a single texture containing every character
    std::unique_ptr<unsigned char> tex(new unsigned char[tex_width*tex_height]);
    // Zero the memory since not all glyphs have the same height
    std::fill(tex.get(), tex.get()+(tex_width*tex_height), 0);
    // We have to do this as a second pass because we dont know the fontsheet width until
    // all of the glyphs have been rendered and stored
    for (const auto& glyph : charset_)
    {
        for (unsigned int i = 0; i < glyph.pixels.size(); i++)
        {
            units::pixel x = glyph.tex_offset + i % glyph.width;
            units::pixel y = i / glyph.width;
            tex.get()[x + y * tex_width] = glyph.pixels[i];
        }
    }

    // Converge all our pixel data into a handy dandy struct
    PixelData font;
    font.pixels = std::move(tex);
    font.width = tex_width;
    font.height = tex_height;
    // Single channel monochrome
    font.bits = PixelData::A8;
    // No compression or mipmaps
    font.format = PixelData::RAW;
    // Make it a sprite!
    fontsheet_.reset(new Sprite(&font, context));

    if (FT_Done_FreeType(library) != 0)
    {
        throw "uh oh";
    }
    return;
}

// Defined in .cpp to allow for destruction of glyphs
Font::~Font()
{
}

const MeshData* Font::BuildMesh(unsigned char letter, units::subpixel x, units::subpixel y, Box crop)
{
    // Pointer to avoid expensive copying
    const Glyph* g;
    // In case someone tries to render a string using chars we dont have
    try
    {
        g = &charset_[letter];
    }
    catch (...)
    {
        return nullptr;
    }

    // How far to advance cursor for next letter
    advance_ = g->x_advance;

    // Sprite dimensions
    Box s(x + units::pixel_to_subpixel(g->x_offset),
          y - units::pixel_to_subpixel(g->y_offset + g->height),
          units::pixel_to_subpixel(g->width),
          units::pixel_to_subpixel(g->height));
    // Texture dimensions
    Box t(g->tex_offset, 0, g->width, g->height);

    // TODO: Refactor these? Kind of annoying since they access diff struct members
    if (crop.w != 0)
    {
        // Is this letter completely cropped?
        if (s.x + s.w <= crop.x || s.x >= crop.x + crop.w)
        {
            return nullptr;
        }
        units::subpixel crop_left = crop.x - s.x;
        units::subpixel crop_right = (s.x + s.w) - (crop.x + crop.w);
        units::subpixel crop_max = std::max(crop_left, crop_right);
        s.x = std::max(s.x, crop.x);
        s.w = s.w - std::max(0.0f, crop_max);
        t.x = t.x + std::max(0.0f, crop_left);
        t.w = s.w;
    }

    if (crop.h != 0)
    {
        // Is this letter completely cropped?
        if (s.y + s.h <= crop.y || s.y >= crop.y + crop.h)
        {
            return nullptr;
        }
        units::subpixel crop_top = crop.y - s.y;
        units::subpixel crop_bottom = (s.y + s.h) - (crop.y + crop.h);
        units::subpixel crop_max = std::max(crop_top, crop_bottom);
        s.y = std::max(s.y, crop.y);
        s.h = s.h - std::max(0.0f, crop_max);
        t.y = t.y + std::max(0.0f, crop_top);
        t.h = s.h;
    }

    // Setup the character sprites position and texture
    fontsheet_->set_pos(s);
    fontsheet_->set_subtexture(t);

    return &fontsheet_.get()->mesh();
}

const MeshData* Font::BuildMesh(unsigned char letter, units::subpixel x, units::subpixel y)
{
    static const Box no_crop = Box(0, 0, 0, 0);
    return BuildMesh(letter, x, y, no_crop);
}

units::pixel Font::cursor_offset(unsigned char letter) const
{
    // In case someone tries to calculate a string using chars we dont have
    try
    {
        // Pointer to avoid expensive copying
        const Glyph* g = &charset_[letter];
        return g->x_offset;
    }
    catch (...)
    {
        return 0;
    }
}

// TODO: Might need to make this const char* for perf later, if its used a lot
units::pixel Font::string_width(std::string string, bool trim_whitespace) const
{
    if (trim_whitespace)
    {
        while (string.size() > 0 && *string.begin() == ' ')
        {
            string.erase(string.begin());
        }
        while (string.size() > 0 && *(string.end()-1) == ' ')
        {
            string.erase(string.end()-1);
        }
    }

    units::pixel pixel_width = 0;
    // Faster when done old way
    for (auto i = 0; i < string.length(); i++)
    {
        // Pointer to avoid expensive copying
        const Glyph* g;
        // In case someone tries to calculate a string using chars we dont have
        try
        {
            g = &charset_[string[i]];
        }
        catch (...)
        {
            return 0;
        }
        // How far to advance cursor for next letter
        pixel_width += g->x_advance;

        if (trim_whitespace)
        {
            if (i == 0)
            {
                // Leftward padding of the first character
                pixel_width -= g->x_offset;
            }
            if (i == string.length() - 1)
            {
                // Rightward padding of the last character
                pixel_width -= g->x_advance - (g->x_offset + g->width);
            }
        }
    }

    return pixel_width;
}

units::pixel Font::string_width(std::string string) const
{
    return string_width(string, true);
}

std::vector<std::string> Font::string_wrap(std::string string, units::pixel max_width)
{
    std::vector<std::string> broken_strings;

    units::pixel pixel_width = 0;
    std::size_t last_break = 0;
    // Faster when done old way
    for (auto i = 0; i < string.length(); i++)
    {
        // Pointer to avoid expensive copying
        const Glyph* g;
        // In case someone tries to calculate a string using chars we dont have
        try
        {
            g = &charset_[string[i]];
        }
        catch (...)
        {
            return std::vector<std::string>();
        }
        // Out of bounds or newline, slice the string
        if ((pixel_width + g->x_advance > max_width && pixel_width > 0) ||
            string[i] == '\n')
        {
            broken_strings.push_back(string.substr(last_break, i - last_break));
            pixel_width = 0;
            last_break = i;
        }

        // How far to advance cursor for next letter
        pixel_width += g->x_advance;
    }

    // Still some left over string to append
    if (last_break < string.length())
    {
        broken_strings.push_back(string.substr(last_break));
    }

    return broken_strings;
}

std::vector<std::string> Font::string_wrap(ColourString string, units::pixel max_width)
{
    std::vector<std::string> broken_strings;

    units::pixel pixel_width = 0;
    std::size_t last_break = 0;
    // Stores our position in "full_string", skipping ahead when we find colour codes
    int i = 0;
    const auto& full_string = string.raw_str();
    Vector4 current_colour = string.base_colour();
    bool current_base = true;
    for (const auto& f : string.fragments())
    {
        for (const auto& c : f.text)
        {
            current_base = f.is_base;
            // Pointer to avoid expensive copying
            const Glyph* g;
            // In case someone tries to calculate a string using chars we dont have
            try
            {
                g = &charset_[full_string[i]];
            }
            catch (...)
            {
                return std::vector<std::string>();
            }
            // Out of bounds or newline, slice the string
            if ((pixel_width + g->x_advance > max_width && pixel_width > 0) ||
                full_string[i] == '\n')
            {
                std::string prefix = (current_base ? "" : ColourString::MakeColourCode(current_colour));
                broken_strings.push_back(prefix + full_string.substr(last_break, i - last_break));
                pixel_width = 0;
                last_break = i;
                if (full_string[i] == '\n')
                {
                    last_break++;
                }
                current_colour = f.colour;
            }

            // How far to advance cursor for next letter
            pixel_width += g->x_advance;

            i++;
        }
        // New fragment means skip 4 chars for colour code
        i += 4;
    }

    // Still some left over string to append
    if (last_break < full_string.length())
    {
        std::string prefix = (current_base ? "" : ColourString::MakeColourCode(current_colour));
        broken_strings.push_back(prefix + full_string.substr(last_break));
    }

    return broken_strings;
}

int Font::advance()
{
    units::pixel ret = advance_;
    advance_ = 0;
    return ret;
}

units::pixel Font::letter_height() const
{
    return letter_height_;
}

units::pixel Font::line_height() const
{
    return line_height_;
}

units::pixel Font::pixel_size() const
{
    return pixel_size_;
}

const TextureResource* Font::texture() const
{
    return fontsheet_->texture();
}
} // namespace gui
} // namespace blons