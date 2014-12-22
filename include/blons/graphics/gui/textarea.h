#ifndef BLONSTECH_GRAPHICS_GUI_TEXTAREA_H_
#define BLONSTECH_GRAPHICS_GUI_TEXTAREA_H_

// Public Includes
#include <blons/graphics/gui/control.h>
#include <blons/math/animation.h>
#include <blons/system/timer.h>

namespace blons
{
namespace gui
{
// Forward declarations
class Label;

////////////////////////////////////////////////////////////////////////////////
/// \brief UI element for rendering scrollable blocks of text
////////////////////////////////////////////////////////////////////////////////
class Textarea : public Control
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes members with supplied values. Meant to be used by
    /// gui::Window::MakeTextarea
    ///
    /// \param pos Position and dimensions of the textarea
    /// \param style Type of font to be rendered by the textarea
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    Textarea(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls Textarea(Box, FontStyle, Manager*, Window*) with a default
    /// style of `FontStyle::LABEL`
    ////////////////////////////////////////////////////////////////////////////////
    Textarea(Box pos, Manager* parent_manager, Window* parent_window)
        : Textarea(pos, Skin::FontStyle::LABEL, parent_manager, parent_window) {}
    ~Textarea() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context) override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic to detect scrolling.
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the textarea
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Appends text to the end of the textarea with a trailing newline.
    /// Follows gui::ColourString formatting rules
    ///
    /// \param text Text to append
    ////////////////////////////////////////////////////////////////////////////////
    void AddLine(std::string text);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Appends text to the end of the textarea. Follows gui::ColourString
    /// formatting rules
    ///
    /// \param text Text to append
    ////////////////////////////////////////////////////////////////////////////////
    void AddText(std::string text);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Removes all text from the textarea
    ////////////////////////////////////////////////////////////////////////////////
    void Clear();

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates mesh for the body and appends it to the textarea's draw queue
    ///
    /// \param t Region of the UI skin to render
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void RenderBody(const Skin::Layout::Textarea& t, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates mesh for the text and appends it to the textarea's draw queue
    ///
    /// \param t Region of the UI skin to render, containing text colour
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void RenderText(const Skin::Layout::Textarea& t, RenderContext& context);

private:
    void MoveScrollOffset(units::pixel delta, bool smooth);

    Skin::FontStyle font_style_;

    // New lines appear on the top, or bottom
    bool newest_top_;
    void GenLabel(std::string text);
    std::string text_;
    std::vector<std::unique_ptr<Label>> lines_;
    units::pixel line_height_;
    units::pixel padding_;

    // What is rendered
    units::pixel scroll_offset_;
    // For animations (scroll_offset_ approaches scroll_destination_)
    units::pixel scroll_destination_;
    Animation scroll_animation_;
    Timer scroll_timer_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Textarea
/// \ingroup gui
///
/// The textarea element can be created from or attached to any window. Supports
/// gui::ColourString formatting.
///
/// ### Example:
/// \code
/// // Adding a textarea to a window
/// auto gui = graphics->gui();
/// auto window = gui->MakeWindow("window_id", 0, 0, 300, 300, "Window title");
/// auto textarea = window->MakeTextarea(10, 40, 200, 200);
///
/// // Outputting a line of text
/// textarea->AddLine("Text to be outputted!");
///
/// // Outputting a line of text through several calls
/// textarea->AddText("Text to");
/// textarea->AddText("be outputted! ");
/// textarea->AddText("We must add the newline at the end of this manually\n");
///
/// // Outputting coloured text
/// textarea->AddLine("$f00This will be red text!");
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_TEXTAREA_H_