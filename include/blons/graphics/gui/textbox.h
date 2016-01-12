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

#ifndef BLONSTECH_GRAPHICS_GUI_TEXTBOX_H_
#define BLONSTECH_GRAPHICS_GUI_TEXTBOX_H_

// Public Includes
#include <blons/graphics/gui/control.h>
#include <blons/system/timer.h>

namespace blons
{
namespace gui
{
// Forward delcarations
class Label;

////////////////////////////////////////////////////////////////////////////////
/// \brief UI element for taking typed user input
////////////////////////////////////////////////////////////////////////////////
class Textbox : public Control
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes members with supplied values. Meant to be used by
    /// gui::Window::MakeTextbox
    ///
    /// \param pos Position and dimensions of the textbox
    /// \param style Type of font to be rendered by the textbox
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    Textbox(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls Textbox(Box, Skin::FontStyle, Manager*, Window*) with a default
    /// style of `Skin::FontStyle::LABEL`
    ////////////////////////////////////////////////////////////////////////////////
    Textbox(Box pos, Manager* parent_manager, Window* parent_window)
        : Textbox(pos, Skin::FontStyle::LABEL, parent_manager, parent_window) {}
    ~Textbox() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context) override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic to detect typing and text manipulation
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the textbox
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a callback to be invoked when the Return key is pressed while
    /// the textbox is active. The callback takes a single argument containing a
    /// pointer to the calling textbox and returns void.
    ///
    /// \param callback Function to be called when Return is pressed. Accepts a
    /// `blons::gui::Textbox*` pointer and returns void
    ////////////////////////////////////////////////////////////////////////////////
    void set_callback(std::function<void(Textbox*)> callback);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief True if the textbox is currently focused and accepting keyboard input
    ///
    /// \return Focus state
    ////////////////////////////////////////////////////////////////////////////////
    bool focus() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets whether the textbox is focused and can accept keyboard input
    ///
    /// \param focus Focus state
    ////////////////////////////////////////////////////////////////////////////////
    void set_focus(bool focus);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Returns the starting and ending index of the highlighted region
    /// within the text of the box. First value will always be the location of the
    /// text cursor and as such may be greater, equal or less than the ending index.
    ///
    /// \return Start and end index
    ////////////////////////////////////////////////////////////////////////////////
    std::pair<std::size_t, std::size_t> highlight() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the highlighted region of text within the textbox. Accepts 2
    /// indices to determine the highlighted region. Numbers greater than the length
    /// of the string will be placed at the end. Negative values will wrap around
    /// from the back of the string. For example a call of `set_highlight(0, -1);`
    /// would highlight the entire string. The indices can be provided in either
    /// order, with the cursor argument placing the location of the actual text
    /// cursor within the box. Supplying 2 identical values will simply place the
    /// text cursor where specified.
    ///
    /// \param cursor Index of text cursor
    /// \param end Index of opposing end of highlight region
    ////////////////////////////////////////////////////////////////////////////////
    void set_highlight(int cursor, int end);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the text that has been typed into the textbox
    ///
    /// \return Text currently inside textbox
    ////////////////////////////////////////////////////////////////////////////////
    std::string text() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the text inside of the current textbox. Does not parse colour
    /// formatting
    ///
    /// \param text Text to set
    ////////////////////////////////////////////////////////////////////////////////
    void set_text(std::string text);

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates mesh for the body and appends it to the textbox's draw queue
    ///
    /// \param t Region of the UI skin to render
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void RenderBody(const Skin::Layout::Textbox& t, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates mesh for the blinking text cursor and appends it to the
    /// textbox's draw queue
    ///
    /// \param cursor Region of the UI skin to render
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void RenderCursor(const Box& cursor, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates mesh for the text and appends it to the textbox's draw queue
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void RenderText(RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Takes a handle to input events and returns a modified event queue
    /// with "repeat keys" injected to simulate held keys being hit repeatedly.
    ///
    /// \param input Handle to the current input events
    /// \return A sequenced vector of input events
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<Input::Event> Textbox::GetEventsWithRepeats(const Input& input);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Handles textbox input logic for dealing with mouse clicks
    ///
    /// \param input Handle to the current input events
    ////////////////////////////////////////////////////////////////////////////////
    void OnMouseDown(const Input& input);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Handles textbox input logic for dealing with mouse release
    ///
    /// \param input Handle to the current input events
    ////////////////////////////////////////////////////////////////////////////////
    void OnMouseUp(const Input& input);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Handles textbox input logic for dealing with mouse movement
    ///
    /// \param input Handle to the current input events
    ////////////////////////////////////////////////////////////////////////////////
    void OnMouseMove(const Input& input);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Handles textbox input logic for dealing with key presses
    ///
    /// \param input Handle to the current input events
    /// \param key The current key being pressed
    /// \param mods Modifiers determining which shift, ctrl, etc keys are currently
    /// active
    ////////////////////////////////////////////////////////////////////////////////
    void OnKeyDown(const Input& input, const Input::KeyCode key, Input::Modifiers mods);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Handles textbox input logic for dealing with key releases
    ///
    /// \param input Handle to the current input events
    /// \param key The current key being pressed
    /// \param mods Modifiers determining which shift, ctrl, etc keys are currently
    /// active
    ////////////////////////////////////////////////////////////////////////////////
    void OnKeyUp(const Input& input, const Input::KeyCode key, Input::Modifiers mods);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Handles scrolling text side to side when highlight dragging out of
    /// textbox boundaries. Should be called once per Textbox::Update.
    ///
    /// \param input Handle to the current input events
    ////////////////////////////////////////////////////////////////////////////////
    void UpdateHighlightScroll(const Input& input);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the callback used for this textbox
    ///
    /// \return Callback function
    ////////////////////////////////////////////////////////////////////////////////
    std::function<void(Textbox*)> callback() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the font style used for this textbox
    ///
    /// \return Font style
    ////////////////////////////////////////////////////////////////////////////////
    Skin::FontStyle font_style() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a pointer to the textbox's label used for rendering
    /// input text
    ///
    /// \return Label pointer
    ////////////////////////////////////////////////////////////////////////////////
    Label* label() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the amount of text padding used by the textbox in pixels
    ///
    /// \return Text padding
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel padding() const;

private:
    // Helper functions
    std::string::iterator NearestCursorPos(units::pixel mouse_x);
    void SetCursorPos(std::string::iterator cursor);
    units::subpixel LabelOffset(std::string::iterator cursor);

    std::string text_;
    Skin::FontStyle font_style_;
    std::string::iterator cursor_, highlight_;
    Timer cursor_blink_, drag_highlight_scrolling_;
    struct
    {
        Timer timer;
        Input::KeyCode code;
    } key_repeat_;
    std::unique_ptr<Label> text_label_;
    std::function<void(Textbox*)> callback_;
    bool active_, drag_highlighting_;
    // Cached skin values
    units::pixel padding_, edge_width_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Textbox
/// \ingroup gui
///
/// The textbox element can be created from or attached to any window. Supports
/// a user definable callback that is invoked when the Return key is pressed.
///
/// ### Example:
/// \code
/// // Adding a textbox to a window
/// auto gui = graphics->gui();
/// auto window = gui->MakeWindow("window_id", 0, 0, 300, 300, "Window title");
/// auto textbox = window->MakeTextbox(10, 40, 250, 40);
///
/// // Setting a callback that logs whatever is typed
/// textbox->set_callback([](blons::gui::Textbox* textbox)
/// {
///     blons::log::Debug("%s\n", textbox->text().c_str());
///     textbox->set_text("");
/// });
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_TEXTBOX_H_