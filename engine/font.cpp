#include "font.h"

// Includes
#include <FreeType2\include\ft2build.h>
#include FT_FREETYPE_H
// Local Includes
#include "render.h"

namespace blons
{
struct Font::Glyph
{
    // Needed for std::map storage
    Glyph() = default;
    // This constructor's really just for refactoring code into smaller functions
    Glyph(unsigned char letter, FT_Face font_face, unsigned int texture_offset);
    // 8-bit monochrome bitmap of character data
    std::vector<unsigned char> pixels;
    // Width and height of bitmap
    unsigned int width, height;
    // Offset from origin in fontsheet texture
    unsigned int tex_offset;
    // Offset from previously rendered character
    int x_offset, y_offset;
};

Font::Glyph::Glyph(unsigned char letter, FT_Face font_face, unsigned int texture_offset)
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
    x_offset = font_face->glyph->advance.x / 64 - width;
    y_offset = font_face->glyph->metrics.horiBearingY / 64 - height;

    for (int y = 0; y < bitmap.rows; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            pixels.push_back(*(bitmap.buffer++));
        }
        // Accounts for bitmap padding
        bitmap.buffer += (bitmap.pitch - bitmap.width);
    }
}

Font::Font(const char* font_filename, int pixel_size, RenderContext& context)
{
    fontsheet_ = nullptr;
    pixel_size_ = pixel_size;

    FT_Library library;
    FT_Face face;

    if (FT_Init_FreeType(&library) != 0)
    {
        throw "Couldn't initialize font library";
    }

    if (FT_New_Face(library, font_filename, 0, &face) != 0)
    {
        throw "Couldn't load font file";
    }

    if (FT_Set_Pixel_Sizes(face, 0, pixel_size_) != 0)
    {
        throw "Couldn't set pixel size";
    }

    // Used to find glyph in texture fontsheet later
    unsigned int tex_width = 0;
    // Used to determine how tall font texture must be
    unsigned int tex_height = 0;
    // For every drawable character, get a bitmap rendering of the letter and store it
    for (auto& c : kAvailableCharacters)
    {
        Glyph g(c, face, tex_width);
        charset_[c] = g;
        tex_width += g.width;
        if (g.height > tex_height)
        {
            tex_height = g.height;
        }
    }

    // Generate a single texture containing every character
    std::unique_ptr<unsigned char> tex(new unsigned char[tex_width*tex_height]);
    // Zero the memory since not all glyphs have the same height
    std::fill(tex.get(), tex.get()+(tex_width*tex_height), 0);
    // We have to do this as a second pass because we dont know the fontsheet width until
    // all of the glyphs have been rendered and stored
    for (auto& it : charset_)
    {
        auto glyph = &it.second;
        auto offset = glyph->tex_offset;
        for (unsigned int i = 0; i < glyph->pixels.size(); i++)
        {
            int x = offset + i % glyph->width;
            int y = i / glyph->width;
            tex.get()[x + y * tex_width] = glyph->pixels[i];
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
    fontsheet_ = std::unique_ptr<Sprite>(new Sprite(&font, context));

    if (FT_Done_FreeType(library) != 0)
    {
        throw "uh oh";
    }
    return;
}

Font::~Font()
{
}

bool Font::Render(unsigned char letter, int x, int y, RenderContext& context)
{
    Glyph g;
    // In case someone tries to render a string using chars we dont have
    try
    {
        g = charset_[letter];
    }
    catch (...)
    {
        return false;
    }
    // Setup the character sprites position and texture
    fontsheet_->set_pos(x, y + g.y_offset, g.width, g.height);
    fontsheet_->set_subtexture(g.tex_offset, 0, g.width, g.height);
    // Build the quad and push it to buffer
    fontsheet_->Render(context);
    // How far to advance cursor for next letter
    advance_ = g.width + g.x_offset;
    return true;
}

int Font::advance()
{
    int ret = advance_;
    advance_ = 0;
    return ret;
}

int Font::index_count()
{
    return fontsheet_->index_count();
}

TextureResource* Font::texture()
{
    return fontsheet_->texture();
}
} // namespace blons