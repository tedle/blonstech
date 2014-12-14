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
    /// \param style %Font to use for rendering text
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    Label(Vector2 pos, ColourString text, FontStyle style, Manager* parent_manager, Window* parent_window);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls Label(Vector2, ColourString, FontStyle, Manager*, Window*)
    /// with a default style of `FontStyle::LABEL`
    ////////////////////////////////////////////////////////////////////////////////
    Label(Vector2 pos, ColourString text, Manager* parent_manager, Window* parent_window)
        : Label(pos, text, FontStyle::LABEL, parent_manager, parent_window) {}
    ~Label() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits drawbatches with mesh data & shader inputs to the parent
    /// gui::Manager
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context) override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Does nothing, returns false
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the label
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

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
    FontStyle font_style_;
    ColourString text_;
    bool colour_parsing_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Label
/// \ingroup graphics
///
/// The label can be created from or attached to any window.
///
/// ### Example:
/// \code
/// // Adding a label to a window
/// auto gui = graphics->gui();
/// auto window = gui->MakeWindow("window_id", 0, 0, 200, 200, "Window title");
/// window->MakeLabel(50, 50, "Welcome, gamer! $0f0 Green text here!");
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_LABEL_H_