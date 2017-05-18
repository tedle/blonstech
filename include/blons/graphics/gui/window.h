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

#ifndef BLONSTECH_GRAPHICS_GUI_WINDOW_H_
#define BLONSTECH_GRAPHICS_GUI_WINDOW_H_

// Public Includes
#include <blons/graphics/gui/control.h>
#include <blons/graphics/gui/button.h>
#include <blons/graphics/gui/debugslider.h>
#include <blons/graphics/gui/image.h>
#include <blons/graphics/gui/label.h>
#include <blons/graphics/gui/textarea.h>
#include <blons/graphics/gui/textbox.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Container for UI elements
////////////////////////////////////////////////////////////////////////////////
class Window : public Control
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to determine Window behaviour
    ////////////////////////////////////////////////////////////////////////////////
    enum Type
    {
        DRAGGABLE, ///< Has a titlebar, caption, and can be dragged around
        STATIC,    ///< Renders like a pane, cannot be moved
        INVISIBLE  ///< Renders only child Control%s
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes members with supplied values. Meant to be used by
    /// gui::Manager::MakeWindow
    ///
    /// \param id Unique string to identify and retrieve window by
    /// \param pos Position and dimensions of the window
    /// \param caption String to be displayed in titlebar, only used if type is
    /// `Window::Type::DRAGGABLE`
    /// \param type How the window should behave. See gui::Window::Type
    /// \param parent_manager gui::Manager containing this window
    ////////////////////////////////////////////////////////////////////////////////
    Window(std::string id, Box pos, std::string caption, Type type, Manager* parent_manager);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls Window(std::string, Box, std::string, Window::Type, Manager*)
    /// with an empty caption
    ////////////////////////////////////////////////////////////////////////////////
    Window(std::string id, Box pos, Type type, Manager* parent_manager)
        : Window(id, pos, "", type, parent_manager) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls Window(std::string, Box, std::string, Window::Type, Manager*)
    /// with a type of `Window::Type::DRAGGABLE`
    ////////////////////////////////////////////////////////////////////////////////
    Window(std::string id, Box pos, std::string caption, Manager* parent_manager)
        : Window(id, pos, caption, Type::DRAGGABLE, parent_manager) {}
    ~Window() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager for itself and each child Control
    ////////////////////////////////////////////////////////////////////////////////
    void Render() override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic for itself and all child Control%s
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the window
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new blons::gui::Button inside the window
    ///
    /// \param x Where to place the button horizontally in pixels
    /// \param y Where to place the button vertically in pixels
    /// \param width How wide the button should be in pixels
    /// \param height How tall the button should be in pixels
    /// \param label Caption text displayed on top of the button. Follows
    /// gui::ColourString formatting rules.
    /// \return Pointer to the created button. This memory is owned by the
    /// gui::Window and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    Button* MakeButton(units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string label);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new blons::gui::Image inside the window
    ///
    /// \param x Where to place the image horizontally in pixels
    /// \param y Where to place the image vertically in pixels
    /// \param width How wide the image should be in pixels
    /// \param height How tall the image should be in pixels
    /// \param filename Filename on disk of the image to be loaded
    /// \return Pointer to the created image. This memory is owned by the
    /// gui::Window and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    Image* MakeImage(units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string filename);

    ////////////////////////////////////////////////////////////////////////////////
    /// \copybrief MakeImage(units::pixel,units::pixel,units::pixel,units::pixel,std::string)
    ///
    /// \param x Where to place the image horizontally in pixels
    /// \param y Where to place the image vertically in pixels
    /// \param width How wide the image should be in pixels
    /// \param height How tall the image should be in pixels
    /// \param pixel_data PixelData containing image
    /// \return Pointer to the created image. This memory is owned by the
    /// gui::Window and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    Image* MakeImage(units::pixel x, units::pixel y, units::pixel width, units::pixel height, const PixelData& pixel_data);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new blons::gui::DebugSlider inside the window
    ///
    /// \param x Where to place the button horizontally in pixels
    /// \param y Where to place the button vertically in pixels
    /// \param width How wide the button should be in pixels
    /// \param height How tall the button should be in pixels
    /// \param min Minimum value that can be held by debug slider
    /// \param max Maximum value that can be held by debug slider
    /// \param step Minimum interval that value may be changed by
    /// \return Pointer to the created debug slider. This memory is owned by the
    /// gui::Window and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    DebugSlider* MakeDebugSlider(units::pixel x, units::pixel y, units::pixel width, units::pixel height, float min, float max, float step);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new blons::gui::Label inside the window
    ///
    /// \param x Where to place the label horizontally in pixels
    /// \param y Where to place the label vertically in pixels
    /// \param text Text for the label to display. Follows gui::ColourString
    /// formatting rules.
    /// \return Pointer to the created label. This memory is owned by the
    /// gui::Window and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    Label* MakeLabel(units::pixel x, units::pixel y, std::string text);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new blons::gui::Textarea inside the window
    ///
    /// \param x Where to place the textarea horizontally in pixels
    /// \param y Where to place the textarea vertically in pixels
    /// \param width How wide the textarea should be in pixels
    /// \param height How tall the textarea should be in pixels
    /// \return Pointer to the created textarea. This memory is owned by the
    /// gui::Window and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    Textarea* MakeTextarea(units::pixel x, units::pixel y, units::pixel width, units::pixel height);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new blons::gui::Textbox inside the window
    ///
    /// \param x Where to place the textbox horizontally in pixels
    /// \param y Where to place the textbox vertically in pixels
    /// \param width How wide the textbox should be in pixels
    /// \param height How tall the textbox should be in pixels
    /// \return Pointer to the created textbox. This memory is owned by the
    /// gui::Window and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    Textbox* MakeTextbox(units::pixel x, units::pixel y, units::pixel width, units::pixel height);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the unique string used to identify this window
    ///
    /// \return Unique string ID
    ////////////////////////////////////////////////////////////////////////////////
    const std::string id() const;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief A list containing every child Control to be rendered by the window
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<std::unique_ptr<Control>> controls_;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates mesh for the body and appends it to the window's draw queue
    ////////////////////////////////////////////////////////////////////////////////
    void RenderBody();

private:
    const std::string id_;
    Type type_;

    // For draggable windows
    bool dragging_;
    Vector2 drag_offset_;
    std::unique_ptr<Label> caption_;
};
} // namepsace GUI
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Window
/// \ingroup gui
///
/// ### Example:
/// \code
/// // Retrieving the gui::Manager
/// auto gui = graphics->gui();
///
/// // Creating a Window
/// auto window = gui->MakeWindow("some id", 0, 0, 300, 300, "Window title!");
///
/// // Retrieving the Window later
/// window = gui->window("some id");
///
/// // Adding a button to the window
/// window->MakeButton(10, 150, 120, 40, "Hey!");
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_WINDOW_H_