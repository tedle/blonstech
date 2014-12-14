#ifndef BLONSTECH_GRAPHICS_GUI_FONT_H_
#define BLONSTECH_GRAPHICS_GUI_FONT_H_

// Includes
#include <vector>
// Public Includes
#include <blons/math/units.h>
#include <blons/graphics/sprite.h>
#include <blons/graphics/gui/colourstring.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Stores font files (.ttf, etc) as sprites for easy rendering to the
/// screen.
////////////////////////////////////////////////////////////////////////////////
class Font
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Loads the supplied font file into a spritesheet, marking the position
    /// of each character. Will throw a const char* (**temporary**) on failure.
    ///
    /// \param font_filename Location of the font file to load
    /// \param pixel_size %Font size in pixels, same as in other programs
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Font(std::string font_filename, units::pixel pixel_size, RenderContext& context);
    ~Font();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief List of characters that will be inserted into the spritesheet.
    ////////////////////////////////////////////////////////////////////////////////
    const std::string kAvailableCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                             "abcdefghijklmnopqrstuvwxyz"
                                             "`~0123456789!@#$%^&*()"
                                             " ,./<>?;':\"[]\\{}|-=_+";

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a supplied letter and position, returning a mesh that
    /// contains the position and UV coordinates to render this letter from the
    /// internal spritesheet (retrieved using Font::texture()). The returned mesh
    /// data is volatile and will likely change on subsequent calls to BuildMesh,
    /// so make a copy if persistant storage is needed.
    ///
    /// \param letter ASCII value of character to render
    /// \param x Horizontal position in subpixel resolution to render character at
    /// \param y Vertical position in subpixel resolution to render character at
    /// \param crop Boundaries within which the character must render. A width 0
    /// prevents horizontal cropping. A height of 0 prevents vertical cropping.
    /// \return Returns mesh data for rendering, or nullptr for completely cropped
    /// letters
    ////////////////////////////////////////////////////////////////////////////////
    const MeshData* BuildMesh(unsigned char letter, units::subpixel x, units::subpixel y, Box crop);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls BuildMesh(unsigned char, units::subpixel, units::subpixel, Box)
    /// with a default crop value of 0, resulting in no crop.
    ////////////////////////////////////////////////////////////////////////////////
    const MeshData* BuildMesh(unsigned char letter, units::subpixel x, units::subpixel y);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the offset in pixels of how far a text cursor should render
    /// left of a given letter.
    ///
    /// \param letter Letter to retrieve offset for
    /// \return Cursor offset in pixels
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel cursor_offset(unsigned char letter) const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the width in pixels of how wide a given string is when
    /// rendered.
    ///
    /// \param string String to calculate width of
    /// \param trim_whitespace If true ignore all leading and trailing whitespace
    /// characters as well as subtract any padding from the first and last real
    /// character.
    /// \return String width in pixels
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel string_width(std::string string, bool trim_whitespace) const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls string_width(std::string, bool) with trim_whitespace set to
    /// true.
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel string_width(std::string string) const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a string and breaks it into segments either when its rendered
    /// width would be longer than the supplied maximum, or when a newline is found.
    ///
    /// \param string String to wrap
    /// \param max_width Max width any returned string segment may be when rendered
    /// \return A vector of string segments, ordered the same as was supplied
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<std::string> string_wrap(std::string string, units::pixel max_width);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a string and breaks it into segments either when its rendered
    /// width would be longer than the supplied maximum, or when a newline is found.
    /// Colour codes are not counted towards the width of a string, as well a colour
    /// code is inserted to the beginning of every string after the first to
    /// maintain colours across several lines.
    ///
    /// \param string String to wrap
    /// \param max_width Max width any returned string segment may be when rendered
    /// \return A vector of string segments, ordered the same as was supplied
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<std::string> string_wrap(ColourString string, units::pixel max_width);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the distance, in pixels, to advance a cursor after each
    /// character is rendered. Should be called **once** after any call to
    /// Font::BuildMesh. The value will be 0 for any calls afterward.
    ///
    /// \return Distance in pixels to advance cursor
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel advance();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the height in pixels of the tallest letter found between
    /// letters A-Z. Excludes the letters Q & J. Generally used for vertically
    /// centering text.
    ///
    /// \return Height in pixels of tallest applicable character
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel letter_height() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the distance in pixels to advance a cursor vertically on
    /// newline.
    ///
    /// \return Height in pixels of a font's line
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel line_height() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the pixel size of the font. Measures equally to the pixel
    /// size unit used by most other programs.
    ///
    /// \return Size in pixels of the font
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel pixel_size() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the texture resource containing the spritesheet with every
    /// letter on it. Used for rendering letters.
    ///
    /// \return Reference to the font texture
    ////////////////////////////////////////////////////////////////////////////////
    const TextureResource* texture() const;

private:
    struct Glyph;
    std::vector<Glyph> charset_;
    std::unique_ptr<class Sprite> fontsheet_;
    units::pixel pixel_size_;
    units::pixel letter_height_;
    units::pixel line_height_;
    units::pixel advance_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Font
/// \ingroup graphics
///
/// For an easy to use UI element that renders text, see gui::Label.
///
/// ### Example:
/// \code
/// // Rendering a string to the screen as a batch
/// std::string text = "Welcome, gamer!";
/// auto batch = std::make_unique<DrawBatcher>(context);
/// auto font = std::make_unique<Font>("font.ttf", 30, context);
/// int x = 100;
/// int y = 100;
/// for (const auto& c : text)
/// {
///     auto mesh = font->BuildMesh(c, x, y);
///     batch->Append(*mesh, context);
///     x += font->advance();
/// }
/// batch->Render(context);
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_FONT_H_