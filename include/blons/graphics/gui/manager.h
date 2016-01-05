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

#ifndef BLONSTECH_GRAPHICS_GUI_MANAGER_H_
#define BLONSTECH_GRAPHICS_GUI_MANAGER_H_

// Public Includes
#include <blons/math/animation.h>
#include <blons/input/inputtemp.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/graphics/gui/skin.h>
#include <blons/graphics/gui/font.h>
#include <blons/graphics/gui/window.h>
#include <blons/graphics/gui/consolewindow.h>

namespace blons
{
// Forward declarations
class Framebuffer;
class Shader;

namespace gui
{
// Forward declarations
class Label;
class Window;

////////////////////////////////////////////////////////////////////////////////
/// \brief Main class for GUI interaction. Handles the creation and management
/// of Window%s, Font%s and Skin%s, as well as rendering and input.
////////////////////////////////////////////////////////////////////////////////
class Manager
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Inputs to be sent to the UI shader. Paired one-to-one with each
    /// blons::Drawbatcher.
    ////////////////////////////////////////////////////////////////////////////////
    struct DrawCallInputs
    {
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief True if the draw call is rendering text
        ////////////////////////////////////////////////////////////////////////////////
        bool is_text;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Font to use for text rendering
        ////////////////////////////////////////////////////////////////////////////////
        Skin::FontStyle font_style;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Is only applied to text. Values range from 0.0 to 1.0
        ////////////////////////////////////////////////////////////////////////////////
        Vector4 colour;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Only render anything inside this Box. A width of 0 prevents any
        /// horizontal cropping. A height of 0 prevents any vertical cropping.
        ////////////////////////////////////////////////////////////////////////////////
        Box crop;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Renders a linear alpha fade towards the crop box, approaching 0 at
        /// the boundaries.
        ////////////////////////////////////////////////////////////////////////////////
        units::pixel crop_feather;
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes the UI and console window.
    ///
    /// \param screen_width Maximum width of the view screen in pixels
    /// \param screen_height Maximum height of the view screen in pixels
    /// \param ui_shader Shader to be used for rendering Control%s and text
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Manager(units::pixel screen_width, units::pixel screen_height, std::unique_ptr<Shader> ui_shader, RenderContext& context);
    ~Manager();

    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc gui::Skin::LoadFont
    ////////////////////////////////////////////////////////////////////////////////
    bool LoadFont(std::string filename, units::pixel pixel_size, Skin::FontStyle style, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls LoadFont(std::string, units::pixel, Skin::FontStyle, RenderContext&)
    /// with a style of `Skin::FontStyle::DEFAULT`.
    ////////////////////////////////////////////////////////////////////////////////
    bool LoadFont(std::string filename, units::pixel pixel_size, RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new Window for containing Control%s.
    ///
    /// \param id Unique ID to refer to the window by. Used for retrieving the
    /// window at a later time. If the ID is already in use the old window is
    /// deleted and replaced
    /// \param x Where to place the window horizontally in pixels
    /// \param y Where to place the window vertically in pixels
    /// \param width How wide the window should be in pixels
    /// \param height How tall the window should be in pixels
    /// \param caption Text to display on the titlebar. Only applicable if type is
    /// `Window::Type::DRAGGABLE`
    /// \param type Determines how the window will behave. See gui::Window::Type
    /// \return Pointer to the created window. This memory is owned by the
    /// gui::Manager and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    Window* MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption, Window::Type type);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls
    /// MakeWindow(std::string, units::pixel, units::pixel, units::pixel, units::pixel, std::string, Window::Type)
    /// with a type of `Window::Type::DRAGGABLE`
    ////////////////////////////////////////////////////////////////////////////////
    Window* MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls
    /// MakeWindow(std::string, units::pixel, units::pixel, units::pixel, units::pixel, std::string, Window::Type)
    /// with an empty caption
    ////////////////////////////////////////////////////////////////////////////////
    Window* MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, Window::Type type);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders all Window%s and Control%s to the supplied framebuffer.
    /// Generally issue by Graphics class and not by user.
    ///
    /// \param output_buffer Handle to the framebuffer to render to
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(Framebuffer* output_buffer, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders all Window%s and Control%s to the screen. Generally issued
    /// by Graphics class and not by user.
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads GUI to be bound to the supplied rendering context
    ///
    /// \param screen_width Maximum width of the view screen in pixels
    /// \param screen_height Maximum height of the view screen in pixels
    /// \param ui_shader Shader to be used for rendering Control%s and text
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Reload(units::pixel screen_width, units::pixel screen_height, std::unique_ptr<Shader> ui_shader, RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic for all Window%s and Control%s. Generally issued
    /// by user.
    ///
    /// \param input Handle to the current input events
    /// \return True if input was consumed by the GUI
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a registered Window
    ///
    /// \param id Unique ID of the Window to retrieve,
    /// determined during MakeWindow()
    /// \return Pointer to the requested Window, nullptr on failure
    ////////////////////////////////////////////////////////////////////////////////
    Window* window(std::string id);

private:
    // Since we want this class to be accessed by user, we hide these functions
    // despite widgets needing access to them. Kind of hacky to friend it up, but oh well
    friend Control;
    friend Button;
    friend DebugSliderButton;
    friend Label;
    friend Textarea;
    friend ConsoleTextarea;
    friend Textbox;
    friend ConsoleTextbox;
    friend DebugSliderTextbox;
    friend Window;
    friend ConsoleWindow;
    void Init(units::pixel width, units::pixel height, std::unique_ptr<Shader> ui_shader, RenderContext& context);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves an unnused Drawbatcher with the specific shader inputs
    /// for a Control to render to.
    ///
    /// \param inputs The shader settings that will be applied to this Drawbatcher
    /// during the render cycle
    /// \param context Handle to the current rendering context
    /// \return The Drawbatcher for rendering
    ////////////////////////////////////////////////////////////////////////////////
    DrawBatcher* batch(DrawCallInputs inputs, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves an unnused Drawbatcher with shader inputs tailored
    /// towards font rendering.
    ///
    /// \param style gui::Skin::FontStyle to be drawn
    /// \param colour Colour of the text with values ranging from 0.0 to 1.0
    /// \param context Handle to the current rendering context
    /// \return The Drawbatcher for rendering
    ////////////////////////////////////////////////////////////////////////////////
    DrawBatcher* font_batch(Skin::FontStyle style, Vector4 colour, Box crop, units::pixel crop_feather, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves an unnused Drawbatcher with shader inputs tailored
    /// towards element body rendering.
    ///
    /// \param context Handle to the current rendering context
    /// \return The Drawbatcher for rendering
    ////////////////////////////////////////////////////////////////////////////////
    DrawBatcher* control_batch(Box crop, units::pixel crop_feather, RenderContext& context);

    Skin* skin() const;
    Window* active_window() const;
    void set_active_window(Window* window);
    Box screen_dimensions();

    // Vector + batch index stored to recycle as much memory as we can
    std::vector<std::pair<DrawCallInputs, std::unique_ptr<DrawBatcher>>> draw_batches_;
    std::size_t batch_index_ = 0;

    Box screen_dimensions_;
    Matrix ortho_matrix_;

    std::unique_ptr<Shader> ui_shader_;

    std::unique_ptr<Skin> skin_;
    std::vector<std::unique_ptr<Window>> windows_;
    std::unique_ptr<Window> main_window_;
    std::unique_ptr<Window> console_window_;
};
} // namespace gui
} // namespace blons

// TODO: Add LoadFont to the usage example once publically usable
////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Manager
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
///
/// // Render loop
/// while (true)
/// {
///     // Input logic
///     gui->Update(*client->input());
///
///     // Rendering the UI (and everything else)
///     graphics->Render();
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_MANAGER_H_
