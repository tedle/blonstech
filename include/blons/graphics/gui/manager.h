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

#ifndef BLONSTECH_GRAPHICS_GUI_MANAGER_H_
#define BLONSTECH_GRAPHICS_GUI_MANAGER_H_

// Includes
#include <set>
// Public Includes
#include <blons/math/animation.h>
#include <blons/input/inputtemp.h>
#include <blons/graphics/render/shaderdata.h>
#include <blons/graphics/gui/skin.h>
#include <blons/graphics/gui/font.h>
#include <blons/graphics/gui/window.h>
#include <blons/graphics/gui/consolewindow.h>
#include <blons/graphics/gui/debugoverlay.h>

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
    ////////////////////////////////////////////////////////////////////////////////
    Manager(units::pixel screen_width, units::pixel screen_height);
    ~Manager();

    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc gui::Skin::LoadFont
    ////////////////////////////////////////////////////////////////////////////////
    bool LoadFont(std::string filename, units::pixel pixel_size, Skin::FontStyle style);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls LoadFont(std::string, units::pixel, Skin::FontStyle)
    /// with a style of `Skin::FontStyle::DEFAULT`.
    ////////////////////////////////////////////////////////////////////////////////
    bool LoadFont(std::string filename, units::pixel pixel_size);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Adds a user created Window. Memory is owned by the gui::Manager class
    ///
    /// \param window Unique pointer to a Window to be added
    /// \return Raw pointer to the Window just added
    ////////////////////////////////////////////////////////////////////////////////
    Window* AddWindow(std::unique_ptr<Window> window);
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
    /// \brief Adds a user created overlay. Overlays are Window%s that render on top
    /// of the rest of the UI, are invisible, take no input, and cover the entire
    /// screen. Memory is owned by the gui::Manager class
    ///
    /// \param overlay Unique pointer to a overlay Window to be added
    /// \return Raw pointer to the overlay just added
    ////////////////////////////////////////////////////////////////////////////////
    Window* AddOverlay(std::unique_ptr<Window> overlay);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new overlay for containing Control%s to be rendered on the
    /// top layer of the UI.
    ///
    /// \return Pointer to the created overlay. This memory is owned by the
    /// gui::Manager and should **not** be deleted.
    ////////////////////////////////////////////////////////////////////////////////
    Window* MakeOverlay();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders all Window%s and Control%s to the supplied framebuffer.
    /// Generally issue by Graphics class and not by user. If no framebuffer is
    /// supplied the backbuffer is used instead. Drop shadows and blurs can only
    /// be applied when a framebuffer is given.
    ///
    /// \param output_buffer Handle to the framebuffer to render to
    ////////////////////////////////////////////////////////////////////////////////
    void Render(Framebuffer* output_buffer);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders all Window%s and Control%s to the screen. Generally issued
    /// by Graphics class and not by user. Draws to the backbuffer with no blur or
    /// shadows applied.
    ////////////////////////////////////////////////////////////////////////////////
    void Render();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Builds a list of draw calls to be consumed by the Render() function.
    /// Can be run in a separate thread so long as the gui::Manager class is not
    /// accessed until the completion of this function. Will return instantly if
    /// already called since the previous use Render()
    ////////////////////////////////////////////////////////////////////////////////
    void BuildDrawCalls();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Reloads GUI to be bound to the active rendering context
    ///
    /// \param screen_width Maximum width of the view screen in pixels
    /// \param screen_height Maximum height of the view screen in pixels
    ////////////////////////////////////////////////////////////////////////////////
    void Reload(units::pixel screen_width, units::pixel screen_height);

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
    friend Image;
    friend Label;
    friend Textarea;
    friend ConsoleTextarea;
    friend Textbox;
    friend ConsoleTextbox;
    friend DebugSliderTextbox;
    friend Window;
    friend ConsoleWindow;
    friend DebugOverlay;

    void Init(units::pixel width, units::pixel height);

    void SubmitBatch(const Box& pos, const Box& uv, const DrawCallInputs& inputs);
    void SubmitControlBatch(const Box& pos, const Box& uv, const Box& crop, const units::pixel& feather);
    void SubmitImageBatch(const Box& pos, const Box& uv, const Box& crop, const units::pixel& feather, const Image::InternalHandle& image);
    void SubmitFontBatch(const Box& pos, const Box& uv, const Skin::FontStyle& style, const Vector4& colour, const Box& crop, const units::pixel& feather);

    std::unique_ptr<Image::InternalHandle> RegisterInternalImage(const std::string& filename);
    std::unique_ptr<Image::InternalHandle> RegisterInternalImage(const PixelData& pixel_data);
    void FreeInternalImage(Image::InternalHandle* image);

    Skin* skin() const;
    Window* active_window() const;
    void set_active_window(Window* window);
    Box screen_dimensions();

    // Restructured for GPU readability
    struct InternalDrawCallInputs
    {
        Vector4 colour;
        Box pos;
        Box uv;
        Box crop;
        int is_text;
        units::pixel crop_feather;
        int texture_id;
    };

    // Batches are indexed by texture id and avoid costly lookups like with hash tables
    static const int kReservedTextureSlots = 6;
    int TranslateToTextureID(const Skin::FontStyle& style, bool is_text);
    const TextureResource* TextureFromID(int texture_id);
    struct
    {
        // Vector + batch index stored to recycle as much memory as we can
        // Stores draw batches, indexed by texture id
        std::vector<InternalDrawCallInputs> inputs;
        // Stores the queue of images to be rendered
        std::vector<Texture*> image_list;
        // Stores number of draw batches
        std::size_t index = 0;
        // Stores the number of different images to be rendered
        std::size_t image_index = 0;
        // Stores batches on GPU for instanced rendering
        std::unique_ptr<ShaderData<InternalDrawCallInputs>> shader_data;
        // Since we have a limited nubmer of texture slots, we may need to split the
        // draw calls into multiple batches. This stores how many instances there are per draw call
        std::vector<unsigned int> split_markers;
    } batches_;
    // Stores textures used for image controls
    std::set<Texture*> image_handles_;
    // The prim of the hour, gets instance rendered a billion times
    std::unique_ptr<Mesh> quad_mesh_;

    Box screen_dimensions_;
    Matrix ortho_matrix_;
    Matrix blur_ortho_matrix_;
    int max_texture_slots_;

    std::unique_ptr<Shader> ui_shader_;
    std::unique_ptr<Framebuffer> ui_buffer_;
    std::unique_ptr<Shader> blur_shader_;
    std::unique_ptr<Framebuffer> blur_buffer_a_;
    std::unique_ptr<Framebuffer> blur_buffer_b_;
    std::unique_ptr<Shader> composite_shader_;

    std::unique_ptr<Skin> skin_;
    std::vector<std::unique_ptr<Window>> windows_;
    std::vector<std::unique_ptr<Window>> overlays_;
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
