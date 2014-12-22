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
    /// \brief Calls LoadFont(std::string, units::pixel, FontStyle, RenderContext&)
    /// with a style of `FontStyle::DEFAULT`.
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
    /// `WindowType::DRAGGABLE`
    /// \param type Determines how the window will behave. See gui::WindowType
    /// \return Pointer to the created window. This memory is owned by the
    /// gui::Manager and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    Window* MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption, Window::Type type);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls
    /// MakeWindow(std::string, units::pixel, units::pixel, units::pixel, units::pixel, std::string, WindowType)
    /// with a type of `WindowType::DRAGGABLE`
    ////////////////////////////////////////////////////////////////////////////////
    Window* MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls
    /// MakeWindow(std::string, units::pixel, units::pixel, units::pixel, units::pixel, std::string, WindowType)
    /// with an empty caption
    ////////////////////////////////////////////////////////////////////////////////
    Window* MakeWindow(std::string id, units::pixel x, units::pixel y, units::pixel width, units::pixel height, Window::Type type);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders all Window%s and Control%s to the screen. Generally issued
    /// by Graphics class and not by user.
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context);
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
    friend Label;
    friend Textarea;
    friend ConsoleTextarea;
    friend Textbox;
    friend ConsoleTextbox;
    friend Window;
    friend ConsoleWindow;
    void RegisterDrawCall(DrawCallInputs info, DrawBatcher* batch);
    Skin* skin() const;
    Window* active_window() const;
    void set_active_window(Window* window);
    Box screen_dimensions();

    // Raw pointers because cleared every frame
    std::vector<std::pair<DrawCallInputs, DrawBatcher*>> draw_batches_;

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
