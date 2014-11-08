#include "font.h"

// Includes
#include <FreeType2\include\ft2build.h>
#include FT_FREETYPE_H
// Local Includes
#include "render.h"

namespace blons
{
class Font::Glyph
{
public:
    std::vector<unsigned char> pixels_;
    unsigned int width_, height_, tex_offset_, x_offset_, y_offset_;
};

Font::Font(const char* font_filename, RenderContext& context)
{
    fontsheet_ = nullptr;
    int size_px = 16;
    FT_Library library;
    FT_Face face;

    std::string available_chars = "Abcy";

    int error = FT_Init_FreeType(&library);
    if (error)
    {
        throw "uh oh";
    }

    error = FT_New_Face(library, "../../notes/font stuff/test.otf", 0, &face);
    if (error)
    {
        throw "uh oh";
    }

    int screen_dpi_w = 0;
    int screen_dpi_h = 0;
    //error = FT_Set_Char_Size(face, 0, size_px * 16, screen_dpi_w, screen_dpi_h);
    error = FT_Set_Pixel_Sizes(face, 0, size_px);
    if (error)
    {
        throw "uh oh";
    }

    // Used to find glyph in texture fontsheet later
    unsigned int tex_width = 0;
    // Used to determine how tall font texture must be
    unsigned int tex_height = 0;
    for (auto& c : available_chars)
    {
        unsigned int glyph_index = FT_Get_Char_Index(face, c);
        if (!glyph_index)
        {
            throw "uh oh";
        }

        error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        if (error)
        {
            throw "uh oh";
        }
        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (error)
        {
            throw "uh oh";
        }
        if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP ||
            face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
        {
            throw "uh oh";
        }

        FT_Bitmap bitmap = face->glyph->bitmap;
        Glyph g;
        g.width_ = bitmap.width;
        g.height_ = bitmap.rows;
        g.tex_offset_ = tex_width;
        g.x_offset_ = face->glyph->advance.x / 64 - g.width_;
        g.y_offset_ = face->glyph->metrics.horiBearingY / 64;

        for (int y = 0; y < bitmap.rows; y++)
        {
            for (int x = 0; x < bitmap.width; x++)
            {
                g.pixels_.push_back(*(bitmap.buffer++));
            }
            bitmap.buffer += bitmap.pitch;
        }
        charset_[c] = g;
        tex_width += g.width_;
        if (g.height_ > tex_height)
        {
            tex_height = g.height_;
        }
    }

    std::unique_ptr<unsigned char> tex(new unsigned char[tex_width*tex_height]);
    // Zero the memory since not all glyphs have the same height
    std::fill(tex.get(), tex.get()+(tex_width*tex_height), 0);
    for (auto& it : charset_)
    {
        auto glyph = &it.second;
        auto offset = glyph->tex_offset_;
        for (unsigned int i = 0; i < glyph->pixels_.size(); i++)
        {
            int x = offset + i % glyph->width_;
            int y = i / glyph->width_;
            tex.get()[x + y * tex_width] = glyph->pixels_[i];
        }
    }
    PixelData font;
    font.pixels = std::move(tex);
    font.width = tex_width;
    font.height = tex_height;
    font.bits = PixelData::A8;
    font.format = PixelData::RAW;
    fontsheet_ = std::unique_ptr<Sprite>(new Sprite(&font, context));
    return;
}
Sprite* Font::test()
{
    return fontsheet_.get();
}
Font::~Font()
{
}
} // namespace blons