////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
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

#ifndef BLONSTECH_GRAPHICS_GUI_LABEL_H_
#define BLONSTECH_GRAPHICS_GUI_LABEL_H_

// Public Includes
#include <blons/graphics/gui/control.h>
#include <blons/graphics/gui/colourstring.h>

namespace blons
{
namespace gui
{
// Forward declarations
class Manager;

////////////////////////////////////////////////////////////////////////////////
/// \brief UI element for rendering text
////////////////////////////////////////////////////////////////////////////////
class Label : public Control
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes members with supplied values. Meant to be used by
    /// gui::Window::MakeLabel
    ///
    /// \param pos Position of the text
    /// \param text Text do be displayed. Uses gui::ColourString formatting rules
    /// \param style Type of font to be rendered by the label
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    Label(Vector2 pos, ColourString text, Skin::FontStyle style, Manager* parent_manager, Window* parent_window);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls Label(Vector2, ColourString, Skin::FontStyle, Manager*, Window*)
    /// with a default style of `Skin::FontStyle::LABEL`
    ////////////////////////////////////////////////////////////////////////////////
    Label(Vector2 pos, ColourString text, Manager* parent_manager, Window* parent_window)
        : Label(pos, text, Skin::FontStyle::LABEL, parent_manager, parent_window) {}
    ~Label() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits drawbatches with mesh data & shader inputs to the parent
    /// gui::Manager
    ////////////////////////////////////////////////////////////////////////////////
    void Render() override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Does nothing, returns false
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the label
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the font style of the text
    ///
    /// \param style Font style to display with
    ////////////////////////////////////////////////////////////////////////////////
    void set_style(Skin::FontStyle style);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the text to be displayed
    ///
    /// \param text Text to display
    ////////////////////////////////////////////////////////////////////////////////
    void set_text(std::string text);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the text to be displayed
    ///
    /// \param text Text to display
    ////////////////////////////////////////////////////////////////////////////////
    void set_text(ColourString text);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the default colour of text that is used when colour codes aren't
    /// found
    ///
    /// \param colour Colour values ranging from 0.0 to 1.0
    ////////////////////////////////////////////////////////////////////////////////
    void set_text_colour(Vector4 colour);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a reference to the gui::ColourString owned by the label
    ///
    /// \return Reference to label text
    ////////////////////////////////////////////////////////////////////////////////
    const ColourString& text() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets whether the label should parse colour codes or display them
    /// as plain text. Defaults to **true** during construction
    ///
    /// \param colour_parsing True to parse colour codes
    ////////////////////////////////////////////////////////////////////////////////
    void set_colour_parsing(bool colour_parsing);

private:
    Skin::FontStyle font_style_;
    ColourString text_;
    bool colour_parsing_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Label
/// \ingroup gui
///
/// The label can be created from or attached to any window. Supports
/// gui::ColourString formatting.
///
/// ### Example:
/// \code
/// // Adding a label to a window
/// auto gui = graphics->gui();
/// auto window = gui->MakeWindow(0, 0, 200, 200, "Window title", blons::gui::Window::Type::DRAGGABLE);
/// window->MakeLabel(50, 50, "Welcome, gamer! $0f0 Green text here!");
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_LABEL_H_