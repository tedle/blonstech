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

#ifndef BLONSTECH_GRAPHICS_GUI_BUTTON_H_
#define BLONSTECH_GRAPHICS_GUI_BUTTON_H_

// Public Includes
#include <blons/graphics/gui/control.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief UI element that calls a user defined function on click
////////////////////////////////////////////////////////////////////////////////
class Button : public Control
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes members with supplied values. Meant to be used by
    /// gui::Window::MakeButton
    ///
    /// \param pos Position and dimensions of the button
    /// \param label Caption text displayed on top of the button. Follows
    /// gui::ColourString formatting rules.
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    Button(Box pos, std::string label, Manager* parent_manager, Window* parent_window);
    ~Button() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context) override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic to detect mouse clicks.
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the button
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a callback to be invoked when the button is clicked.
    ///
    /// \param callback Function to be called on click
    ////////////////////////////////////////////////////////////////////////////////
    void set_callback(std::function<void()> callback);

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates mesh for the body and appends it to the button's draw queue
    ///
    /// \param b Region of the UI skin to render (active, hover, normal, etc)
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void RenderBody(const Skin::Layout::Button& b, RenderContext& context);

private:
    std::unique_ptr<class Label> label_;
    std::function<void()> callback_;
    bool hover_;
    bool active_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Button
/// \ingroup gui
///
/// The button element can be created from or attached to any window.
///
/// ### Example:
/// \code
/// // Adding a button to a window
/// auto gui = graphics->gui();
/// auto window = gui->MakeWindow("window_id", 0, 0, 200, 200, "Window title");
/// auto button = window->MakeButton(0, 0, 120, 40, "Button!")
///
/// // Setting a click handler
/// auto print = [](){ blons::log::Debug("Button clicked!\n"); };
/// button->set_callback(print);
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_BUTTON_H_