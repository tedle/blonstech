#ifndef BLONSTECH_GRAPHICS_GUI_COLOURSTRING_H_
#define BLONSTECH_GRAPHICS_GUI_COLOURSTRING_H_

// Includes
#include <vector>
// Public Includes
#include <blons/math/math.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Fallback text colour used by various classes when none is supplied.
////////////////////////////////////////////////////////////////////////////////
const Vector4 kDefaultTextColour = Vector4(1.0, 1.0, 1.0, 1.0);

////////////////////////////////////////////////////////////////////////////////
/// \brief Colourable string for UI elements
////////////////////////////////////////////////////////////////////////////////
class ColourString
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Struct for storing coloured text as fragments in the form of
    /// `<RED,"hello ">`, `<BLUE,"world!">`, etc
    ////////////////////////////////////////////////////////////////////////////////
    struct Fragment
    {
        Vector4 colour;
        std::string text;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief True if colour is the base colour of the parent string
        ////////////////////////////////////////////////////////////////////////////////
        bool is_base;
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Parses the supplied text string for colour codes and then splits each
    /// colour into a unique text fragment
    ///
    /// \param text Text to be parsed and stored
    /// \param base_colour Colour to be applied where no colour codes are found.
    /// Defaults to blons::gui::kDefaultTextColour
    ////////////////////////////////////////////////////////////////////////////////
    ColourString(std::string text, Vector4 base_colour);
    ColourString(std::string text) : ColourString(text, kDefaultTextColour) {}
    ColourString() : ColourString("") {}
    ~ColourString() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Accepts a colour in Vector4 format and returns a colour code string.
    ///
    /// \param colour Vector4 with members ranging from [0.0, 1.0]
    /// \return String of the format `$xxx`
    ////////////////////////////////////////////////////////////////////////////////
    static std::string MakeColourCode(Vector4 colour);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Returns the base colour of the current string that is applied when
    /// colour codes aren't found.
    ///
    /// \return Vector4 colour with members ranging from [0.0, 1.0]
    ////////////////////////////////////////////////////////////////////////////////
    const Vector4& base_colour() const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the base colour of the current string that is applied when
    /// colour codes aren't found. Is applied retroactively.
    ///
    /// \param colour Vector4 with members ranging from [0.0, 1.0]
    ////////////////////////////////////////////////////////////////////////////////
    void set_base_colour(Vector4 colour);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Returns a list of text fragments each with a unique colour applied.
    /// List is ordered to be the same as original text string.
    ///
    /// \return List of ColourString::Fragment
    ////////////////////////////////////////////////////////////////////////////////
    const std::vector<Fragment>& fragments() const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Returns the stored text after all colour codes have been parsed and
    /// removed.
    ///
    /// \return Parsed string
    ////////////////////////////////////////////////////////////////////////////////
    const std::string& str() const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Returns the stored text with all colour codes intact.
    ///
    /// \return Unparsed string
    ////////////////////////////////////////////////////////////////////////////////
    const std::string& raw_str() const;

private:
    Vector4 base_colour_;
    std::vector<Fragment> text_fragments_;
    std::string text_;
    std::string raw_text_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::ColourString
/// \ingroup graphics
///
/// Can be formatted with input text using a `$` followed by a 3 character
/// case-insensitive hex code
///
/// ### Example:
/// `$fff` makes a white colour code 
///
/// `default text $f00 red text $0f0 green text!`
///
/// \code
/// // Creating a colour string
/// auto cs1 = blons::gui::ColourString("Normal text! $00f Blue text!");
/// // Creating a colour string with red default text
/// auto cs2 = blons::gui::ColourString("Red text! $fff White text!", Vector4(1.0, 0.0, 0.0, 1.0));
///
/// // Looping over ColourString fragments
/// for (auto frag : cs1.fragments())
/// {
///     auto c = frag.colour;
///     blons::log::Debug("Current colour: %f, %f, %f\n", c.r, c.g, c.b);
///     blons::log::Debug("Current text: %s\n", frag.text.c_str());
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_COLOURSTRING_H_