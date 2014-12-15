#ifndef BLONSTECH_GRAPHICS_GUI_SKIN_H_
#define BLONSTECH_GRAPHICS_GUI_SKIN_H_

// Includes
#include <unordered_map>
#include <memory>
// Public Includes
#include <blons/graphics/gui/font.h>
#include <blons/graphics/render/render.h>

namespace blons
{
// Forward declarations
class Sprite;
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Used to categorize which fonts are used by which UI elements
///   
/// Common uses are:
/// * `FontStyle::DEFAULT`, used when style is unspecified or if a specific
/// style is not loaded in the Skin
/// * `FontStyle::HEADING`, used for title text and Window captions
/// * `FontStyle::LABEL`, used for general text, Button captions, Textbox
/// text, etc
/// * `FontStyle::CONSOLE`, used by the game console
////////////////////////////////////////////////////////////////////////////////
enum FontStyle
{
    DEFAULT,
    HEADING,
    LABEL,
    CONSOLE
};
////////////////////////////////////////////////////////////////////////////////
/// \brief Modularizes the look and feel of the GUI system
////////////////////////////////////////////////////////////////////////////////
class Skin
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Maps the GUI's skin texture pixel coordinates to conveniently named
    /// Box%es.
    ///
    /// Elements are stretched to fit their needed space as aproppriate (left and
    /// right edges are only stretched vertically, bodies are stretched all
    /// directions, etc)
    ////////////////////////////////////////////////////////////////////////////////
    struct Layout
    {

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Struct with Box%es detailing where to find the 4 edges, 4 corners,
        /// and center body of a button in the skin texture, as well as a suggested
        /// colour for rendering text captions
        ////////////////////////////////////////////////////////////////////////////////
        struct Button
        {
            Box top_left;
            Box top;
            Box top_right;
            Box left;
            Box body;
            Box right;
            Box bottom_left;
            Box bottom;
            Box bottom_right;
            Vector4 colour;
        };

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Contains all the types of buttons used by the GUI in various states
        /// (hover, click, standard, etc)
        ////////////////////////////////////////////////////////////////////////////////
        struct ButtonSet
        {
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Displays when a button is not being interacted with
            ////////////////////////////////////////////////////////////////////////////////
            Button normal = Button
            {
                Box(0, 45, 5, 5), // top_left
                Box(6, 45, 1, 5), // top
                Box(8, 45, 5, 5), // top_right
                Box(0, 51, 5, 1), // left
                Box(6, 51, 1, 1), // body
                Box(8, 51, 5, 1), // right
                Box(0, 53, 5, 5), // bottom_left
                Box(6, 53, 1, 5), // bottom
                Box(8, 53, 5, 5), // bottom_right
                Vector4(1.0, 1.0, 1.0, 1.0) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Displays when the mouse hovers over a button
            ////////////////////////////////////////////////////////////////////////////////
            Button hover = Button
            {
                Box(0, 59, 5, 5), // top_left
                Box(6, 59, 1, 5), // top
                Box(8, 59, 5, 5), // top_right
                Box(0, 65, 5, 1), // left
                Box(6, 65, 1, 1), // body
                Box(8, 65, 5, 1), // right
                Box(0, 67, 5, 5), // bottom_left
                Box(6, 67, 1, 5), // bottom
                Box(8, 67, 5, 5), // bottom_right
                Vector4(1.0, 1.0, 1.0, 1.0) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Displays when the mouse is clicking on a button
            ////////////////////////////////////////////////////////////////////////////////
            Button active = Button
            {
                Box(0, 73, 5, 5), // top_left
                Box(6, 73, 1, 5), // top
                Box(8, 73, 5, 5), // top_right
                Box(0, 79, 5, 1), // left
                Box(6, 79, 1, 1), // body
                Box(8, 79, 5, 1), // right
                Box(0, 81, 5, 5), // bottom_left
                Box(6, 81, 1, 5), // bottom
                Box(8, 81, 5, 5), // bottom_right
                Vector4(1.0, 1.0, 1.0, 1.0) // text colour
            };
        } button;

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Struct with Box%es detailing where to find the 4 edges, 4 corners,
        /// and center body of a gui::Textarea in the skin texture, as well as a
        /// suggested colour for rendering displayed text
        ////////////////////////////////////////////////////////////////////////////////
        struct Textarea
        {
            Box top_left;
            Box top;
            Box top_right;
            Box left;
            Box body;
            Box right;
            Box bottom_left;
            Box bottom;
            Box bottom_right;
            Vector4 colour;
        };

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Displayed at all times
        ////////////////////////////////////////////////////////////////////////////////
        Textarea textarea = Textarea
        {
            Box(14, 28, 5, 5), // top_left
            Box(20, 28, 1, 5), // top
            Box(22, 28, 5, 5), // top_right
            Box(14, 34, 5, 1), // left
            Box(20, 34, 1, 1), // body
            Box(22, 34, 5, 1), // right
            Box(14, 36, 5, 5), // bottom_left
            Box(20, 36, 1, 5), // bottom
            Box(22, 36, 5, 5), // bottom_right
            Vector4(1.0, 1.0, 1.0, 1.0) // text colour
        };

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Struct with Box%es detailing where to find the 4 edges, 4 corners,
        /// and center body of a gui::Textbox in the skin texture, as well as a
        /// suggested colour for rendering typed text
        ////////////////////////////////////////////////////////////////////////////////
        struct Textbox
        {
            Box top_left;
            Box top;
            Box top_right;
            Box left;
            Box body;
            Box right;
            Box bottom_left;
            Box bottom;
            Box bottom_right;
            Vector4 colour;
        };

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Contains all the types of textboxes used by the GUI in various states
        /// (active, standard, etc)
        ////////////////////////////////////////////////////////////////////////////////
        struct TextboxSet
        {
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Displays when a textbox is not being interacted with
            ////////////////////////////////////////////////////////////////////////////////
            Textbox normal = Textbox
            {
                Box(14, 0, 5, 5), // top_left
                Box(20, 0, 1, 5), // top
                Box(22, 0, 5, 5), // top_right
                Box(14, 6, 5, 1), // left
                Box(20, 6, 1, 1), // body
                Box(22, 6, 5, 1), // right
                Box(14, 8, 5, 5), // bottom_left
                Box(20, 8, 1, 5), // bottom
                Box(22, 8, 5, 5), // bottom_right
                Vector4(0.8f, 0.8f, 0.8f, 1.0) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Displays when the mouse has activated a textbox and is ready to
            /// accept typed input
            ////////////////////////////////////////////////////////////////////////////////
            Textbox active = Textbox
            {
                Box(14, 14, 5, 5), // top_left
                Box(20, 14, 1, 5), // top
                Box(22, 14, 5, 5), // top_right
                Box(14, 20, 5, 1), // left
                Box(20, 20, 1, 1), // body
                Box(22, 20, 5, 1), // right
                Box(14, 22, 5, 5), // bottom_left
                Box(20, 22, 1, 5), // bottom
                Box(22, 22, 5, 5), // bottom_right
                Vector4(1.0, 1.0, 1.0, 1.0) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Drawn where text insertion takes place. Stretched in all directions
            /// to fit with font specifications
            ////////////////////////////////////////////////////////////////////////////////
            Box cursor = Box(14, 42, 1, 1);
        } textbox;

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Struct with Box%es detailing where to find the 4 edges, 4 corners,
        /// and center body of a gui::Window in the skin texture, as well as a suggested
        /// colour for rendering child gui::Label%s
        ////////////////////////////////////////////////////////////////////////////////
        struct Window
        {
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Struct with Box%es detailing where to find the 2 edges, and body of a
            /// gui::Window's title bar in the skin texture, as well as a suggested colour
            /// for rendering caption text
            ////////////////////////////////////////////////////////////////////////////////
            struct Titlebar
            {
                Box left = Box(0, 0, 5, 30);
                Box center = Box(6, 0, 1, 30);
                Box right = Box(8, 0, 5, 30);
                Vector4 colour = Vector4(0.25f, 0.25f, 0.25f, 1.0);
            } title;
            Box top_left = Box(0, 31, 5, 5);
            Box top = Box(6, 31, 1, 5);
            Box top_right = Box(8, 31, 5, 5);
            Box left = Box(0, 37, 5, 1);
            Box body = Box(6, 37, 1, 1);
            Box right = Box(8, 37, 5, 1);
            Box bottom_left = Box(0, 39, 5, 5);
            Box bottom = Box(6, 39, 1, 5);
            Box bottom_right = Box(8, 39, 5, 5);
            Vector4 colour = Vector4(1.0, 0.0, 1.0, 1.0);
        } window;

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Contains all the types of elements skinned specifically for the game
        /// console
        ////////////////////////////////////////////////////////////////////////////////
        struct Console
        {
            Textarea textarea = Textarea
            {
                Box(56, 0, 10, 10),  // top_left
                Box(67, 0, 1, 10),   // top
                Box(69, 0, 10, 10),  // top_right
                Box(56, 11, 10, 23), // left
                Box(67, 11, 1, 23),  // body
                Box(69, 11, 10, 23), // right
                Box(56, 35, 10, 10), // bottom_left
                Box(67, 35, 1, 10),  // bottom
                Box(69, 35, 10, 10), // bottom_right
                Vector4(0.9f, 0.9f, 0.9f, 1.0) // text colour
            };
            Textbox textbox = Textbox
            {
                Box(30, 0, 10, 10),  // top_left
                Box(41, 0, 1, 10),   // top
                Box(43, 0, 10, 10),  // top_right
                Box(30, 11, 10, 1),  // left
                Box(41, 11, 1, 1),   // body
                Box(43, 11, 10, 1),  // right
                Box(30, 13, 10, 10), // bottom_left
                Box(41, 13, 1, 10),  // bottom
                Box(43, 13, 10, 10), // bottom_right
                Vector4(0.9f, 0.9f, 0.9f, 1.0) // text colour
            };
            Box cursor = Box(56, 48, 1, 1);
        } console;

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Struct with Box%es detailing where to find a pre-rendered drop shadow
        /// for use by other UI elements
        ////////////////////////////////////////////////////////////////////////////////
        struct Dropshadow
        {
            Box bottom_left = Box(82, 0, 10, 40);
            Box bottom = Box(93, 0, 1, 40);
            Box bottom_right = Box(95, 0, 10, 40);
        } dropshadow;
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes nullptr for all Font%s (**temporary**) and a
    /// hard-coded skin texture (**temporary**)
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    Skin(RenderContext& context);
    ~Skin() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Loads a font file into the current Skin.
    ///
    /// \param filename Filename of the font to open
    /// \param pixel_size How large the font should be
    /// \param style Determines which parts of the UI will use the font. Valid
    /// inputs include:
    /// * `FontStyle::DEFAULT`, used when style is unspecified or if a specific
    /// style is not loaded in the Skin
    /// * `FontStyle::HEADING`, used for title text and Window captions
    /// * `FontStyle::LABEL`, used for general text, Button captions, Textbox
    /// text, etc
    /// * `FontStyle::CONSOLE`, used by the game console
    /// \param context Handle to the current rendering context
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool LoadFont(std::string filename, units::pixel pixel_size, FontStyle style, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the font used for the specified FontStyle
    ///
    /// \return Font attached to the specified style. If none has been loaded for
    /// that style then the Font attached to `FontStyle::DEFAULT` is returned
    ////////////////////////////////////////////////////////////////////////////////
    Font* font(FontStyle style);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the Sprite with the current skin texture loaded into it
    ///
    /// \return %Skin texture Sprite
    ////////////////////////////////////////////////////////////////////////////////
    Sprite* sprite() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the skin layout for locating which part of the skin texture
    /// a UI element should render to represent itself
    ///
    /// \return %Skin layout
    ////////////////////////////////////////////////////////////////////////////////
    const Layout* layout() const;

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Stores the current skin texture layout
    ////////////////////////////////////////////////////////////////////////////////
    Layout layout_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Stores the current skin texture
    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Sprite> skin_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Stores any fonts used by the current skin
    ////////////////////////////////////////////////////////////////////////////////
    std::unordered_map<FontStyle, std::unique_ptr<Font>> font_list_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Skin
/// \ingroup gui
///
/// Defines fonts, text colours, and Sprite%s for all Control%s to render with.
///
/// ### Example:
/// \code
/// // A UI element rendered using a skin
/// void DerivesControl::Render(RenderContext& context)
/// {
///     // Find which part of the skin we're supposed to render
///     auto layout = gui_->skin()->layout();
///     auto region = layout->button.normal;
///
///     // Sprite containing the skin texture
///     auto sprite = gui_->skin()->sprite();
///
///     // Batch to send mesh data to
///     auto batch = control_batch(context);
///
///     // Follow the containing Window's position as it's dragged around
///     auto parent_pos = parent_->pos();
///     auto x = pos_.x + parent_pos.x;
///     auto y = pos_.y + parent_pos.y;
///
///     // Render the center body of a button
///     sprite->set_pos(x, y, pos_.w, pos_.h);
///     sprite->set_subtexture(region.body);
///     batch->Append(*sprite->mesh(), context);
///
///     // Submit the batches to gui::Manager
///     RegisterBatches();
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_SKIN_H_