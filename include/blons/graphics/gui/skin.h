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
/// \brief Modularizes the look and feel of the GUI system
////////////////////////////////////////////////////////////////////////////////
class Skin
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Used to categorize which fonts are used by which UI elements
    ////////////////////////////////////////////////////////////////////////////////
    enum FontStyle
    {
        DEFAULT, ///< Used when style is unspecified or if a specific style is not loaded in the Skin
        HEADING, ///< Used for title text and Window captions
        LABEL,   ///< Used for general text, Button captions, Textbox text, etc
        CONSOLE  ///< Used by the game console
    };

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
            Box top_left;     ///< Texture coordinates for the top left region
            Box top;          ///< Texture coordinates for the top edge region
            Box top_right;    ///< Texture coordinates for the top right region
            Box left;         ///< Texture coordinates for the left edge region
            Box body;         ///< Texture coordinates for the body region
            Box right;        ///< Texture coordinates for the right edge region
            Box bottom_left;  ///< Texture coordinates for the bottom left region
            Box bottom;       ///< Texture coordinates for the bottom edge region
            Box bottom_right; ///< Texture coordinates for the bottom right region
            Vector4 colour;   ///< Colour of caption text
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
                Box(1, 81,  1, 1),  // top_left
                Box(1, 81,  1, 1),  // top
                Box(1, 81,  1, 1),  // top_right
                Box(1, 85,  1, 68), // left
                Box(1, 85,  1, 68), // body
                Box(1, 85,  1, 68), // right
                Box(1, 156, 1, 1),  // bottom_left
                Box(1, 156, 1, 1),  // bottom
                Box(1, 156, 1, 1),  // bottom_right
                Vector4(0.9f, 0.91f, 0.93f, 1.0f) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Displays when the mouse hovers over a button
            ////////////////////////////////////////////////////////////////////////////////
            Button hover = Button
            {
                Box(7, 81,  1, 1),  // top_left
                Box(7, 81,  1, 1),  // top
                Box(7, 81,  1, 1),  // top_right
                Box(7, 85,  1, 68), // left
                Box(7, 85,  1, 68), // body
                Box(7, 85,  1, 68), // right
                Box(7, 156, 1, 1),  // bottom_left
                Box(7, 156, 1, 1),  // bottom
                Box(7, 156, 1, 1),  // bottom_right
                Vector4(0.9f, 0.91f, 0.93f, 1.0f) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Displays when the mouse is clicking on a button
            ////////////////////////////////////////////////////////////////////////////////
            Button active = Button
            {
                Box(13, 81,  1, 1),  // top_left
                Box(13, 81,  1, 1),  // top
                Box(13, 81,  1, 1),  // top_right
                Box(13, 85,  1, 68), // left
                Box(13, 85,  1, 68), // body
                Box(13, 85,  1, 68), // right
                Box(13, 156, 1, 1),  // bottom_left
                Box(13, 156, 1, 1),  // bottom
                Box(13, 156, 1, 1),  // bottom_right
                Vector4(0.9f, 0.91f, 0.93f, 1.0f) // text colour
            };
        } button; ///< Layouts for various button states

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Struct with Box%es detailing where to find the 4 edges, 4 corners,
        /// and center body of a gui::Textarea in the skin texture, as well as a
        /// suggested colour for rendering displayed text
        ////////////////////////////////////////////////////////////////////////////////
        struct Textarea
        {
            Box top_left;     ///< Texture coordinates for the top left region
            Box top;          ///< Texture coordinates for the top edge region
            Box top_right;    ///< Texture coordinates for the top right region
            Box left;         ///< Texture coordinates for the left edge region
            Box body;         ///< Texture coordinates for the body region
            Box right;        ///< Texture coordinates for the right edge region
            Box bottom_left;  ///< Texture coordinates for the bottom left region
            Box bottom;       ///< Texture coordinates for the bottom edge region
            Box bottom_right; ///< Texture coordinates for the bottom right region
            Vector4 colour;   ///< Colour of text
            units::pixel padding; ///< Amount of white space between edge and text
        };

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Displayed at all times
        ////////////////////////////////////////////////////////////////////////////////
        Textarea textarea = Textarea
        {
            Box(19, 152, 1, 1), // top_left
            Box(19, 152, 1, 1), // top
            Box(19, 152, 1, 1), // top_right
            Box(19, 152, 1, 1), // left
            Box(19, 81, 1, 68), // body
            Box(19, 152, 1, 1), // right
            Box(19, 152, 1, 1), // bottom_left
            Box(19, 152, 1, 1), // bottom
            Box(19, 152, 1, 1), // bottom_right
            Vector4(0.36f, 0.37f, 0.4f, 1.0f), // text colour
            10 // padding
        };

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Struct with Box%es detailing where to find the 4 edges, 4 corners,
        /// and center body of a gui::Textbox in the skin texture, as well as a
        /// suggested colour for rendering typed text
        ////////////////////////////////////////////////////////////////////////////////
        struct Textbox
        {
            Box top_left;     ///< Texture coordinates for the top left region
            Box top;          ///< Texture coordinates for the top edge region
            Box top_right;    ///< Texture coordinates for the top right region
            Box left;         ///< Texture coordinates for the left edge region
            Box body;         ///< Texture coordinates for the body region
            Box right;        ///< Texture coordinates for the right edge region
            Box bottom_left;  ///< Texture coordinates for the bottom left region
            Box bottom;       ///< Texture coordinates for the bottom edge region
            Box bottom_right; ///< Texture coordinates for the bottom right region
            Vector4 colour;   ///< Colour of text
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
                Box(19, 152, 1, 1), // top_left
                Box(19, 152, 1, 1), // top
                Box(19, 152, 1, 1), // top_right
                Box(19, 152, 1, 1), // left
                Box(19, 81, 1, 68), // body
                Box(19, 152, 1, 1), // right
                Box(19, 152, 1, 1), // bottom_left
                Box(19, 152, 1, 1), // bottom
                Box(19, 152, 1, 1), // bottom_right
                Vector4(0.36f, 0.37f, 0.4f, 1.0f) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Displays when the mouse has activated a textbox and is ready to
            /// accept typed input
            ////////////////////////////////////////////////////////////////////////////////
            Textbox active = Textbox
            {
                Box(19, 152, 1, 1), // top_left
                Box(19, 152, 1, 1), // top
                Box(19, 152, 1, 1), // top_right
                Box(19, 152, 1, 1), // left
                Box(19, 81, 1, 68), // body
                Box(19, 152, 1, 1), // right
                Box(19, 152, 1, 1), // bottom_left
                Box(19, 152, 1, 1), // bottom
                Box(19, 152, 1, 1), // bottom_right
                Vector4(0.36f, 0.37f, 0.4f, 1.0f) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Drawn where text insertion takes place. Stretched in all directions
            /// to fit with font specifications
            ////////////////////////////////////////////////////////////////////////////////
            Box cursor = Box(25, 92, 1, 1);
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Displays on progress based textboxes like gui::DebugSlider
            ////////////////////////////////////////////////////////////////////////////////
            Textbox progress = Textbox
            {
                Box(24, 97, 0, 0), // top_left
                Box(24, 97, 0, 0), // top
                Box(24, 97, 0, 0), // top_right
                Box(24, 97, 0, 0), // left
                Box(24, 97, 0, 0), // body
                Box(24, 97, 0, 0), // right
                Box(25, 81, 1, 6), // bottom_left
                Box(25, 81, 1, 6), // bottom
                Box(25, 81, 1, 6), // bottom_right
                // TODO: Implement overlaid text colour
                Vector4(1.0, 0.0, 0.0, 1.0) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Amount of white space between edge and text
            ////////////////////////////////////////////////////////////////////////////////
            units::pixel padding = 10;
        } textbox; ///< Layouts for various textbox states

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
                Box left = Box(1, 1, 28, 75);   ///< Texture coordinates for the left edge region
                Box center = Box(32, 1, 1, 75); ///< Texture coordinates for the center body region
                Box right = Box(36, 1, 28, 75); ///< Texture coordinates for the right edge region
                Vector4 colour = Vector4(0.52f, 0.54f, 0.61f, 1.0); ///< Colour of the window caption
            } title; ///< Layout for the window's titlebar
            Box top_left = Box(67, 68, 1, 1);     ///< Texture coordinates for the top left region
            Box top = Box(67, 68, 1, 1);          ///< Texture coordinates for the top edge region
            Box top_right = Box(67, 68, 1, 1);    ///< Texture coordinates for the top right region
            Box left = Box(67, 1, 1, 64);         ///< Texture coordinates for the left edge region
            Box body = Box(67, 1, 1, 64);         ///< Texture coordinates for the body region
            Box right = Box(67, 1, 1, 64);        ///< Texture coordinates for the right edge region
            Box bottom_left = Box(67, 72, 1, 1);  ///< Texture coordinates for the bottom left region
            Box bottom = Box(67, 72, 1, 1);       ///< Texture coordinates for the bottom edge region
            Box bottom_right = Box(67, 72, 1, 1); ///< Texture coordinates for the bottom right region
            Vector4 colour = Vector4(1.0, 1.0, 1.0, 1.0); ///< Default colour of labels
        } window; ///< Layout for windows

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Contains all the types of elements skinned specifically for the game
        /// console
        ////////////////////////////////////////////////////////////////////////////////
        struct Console
        {
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Layout for console textareas
            ////////////////////////////////////////////////////////////////////////////////
            Textarea textarea = Textarea
            {
                Box(105, 1, 10, 10),  // top_left
                Box(118, 1, 1, 10),   // top
                Box(122, 1, 10, 10),  // top_right
                Box(105, 14, 10, 23), // left
                Box(118, 14, 1, 23),  // body
                Box(122, 14, 10, 23), // right
                Box(105, 40, 10, 10), // bottom_left
                Box(118, 40, 1, 10),  // bottom
                Box(122, 40, 10, 10), // bottom_right
                Vector4(0.9f, 0.9f, 0.9f, 1.0) // text colour
            };
            ////////////////////////////////////////////////////////////////////////////////
            /// \brief Layout for console textboxes
            ////////////////////////////////////////////////////////////////////////////////
            Textbox textbox = Textbox
            {
                Box(73, 1, 10, 10),  // top_left
                Box(86, 1, 1, 10),   // top
                Box(90, 1, 10, 10),  // top_right
                Box(73, 14, 10, 1),  // left
                Box(86, 14, 1, 1),   // body
                Box(90, 14, 10, 1),  // right
                Box(73, 18, 10, 10), // bottom_left
                Box(86, 18, 1, 10),  // bottom
                Box(90, 18, 10, 10), // bottom_right
                Vector4(0.9f, 0.9f, 0.9f, 1.0f) // text colour
            };
            Vector4 completion_colour = Vector4(0.9f, 0.9f, 1.0f, 0.6f); ///< Colour for tab completion hinting text
            Box cursor = Box(25, 92, 1, 1); ///< Texture coordinates for the text cursor region
        } console; ///< Layout for console controls
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
    /// \param style Determines which parts of the UI will use the font. See
    /// gui::Skin::FontStyle
    /// \param context Handle to the current rendering context
    /// \return True on success
    ////////////////////////////////////////////////////////////////////////////////
    bool LoadFont(std::string filename, units::pixel pixel_size, FontStyle style, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a handle to the font used for the specified Skin::FontStyle
    ///
    /// \return Font attached to the specified style. If none has been loaded for
    /// that style then the Font attached to `Skin::FontStyle::DEFAULT` is returned
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
///     auto batch = control_batch(crop_, feather_, context);
///
///     // Follow the containing Window's position as it's dragged around
///     auto parent_pos = parent_->pos();
///     auto x = pos_.x + parent_pos.x;
///     auto y = pos_.y + parent_pos.y;
///
///     // Render the center body of a button
///     sprite->set_pos(x, y, pos_.w, pos_.h);
///     sprite->set_subtexture(region.body);
///     batch->Append(sprite->mesh(), context);
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_SKIN_H_