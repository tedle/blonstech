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

#ifndef BLONSTECH_GUI_CONSOLETEXTAREA_H_
#define BLONSTECH_GUI_CONSOLETEXTAREA_H_

// Public Includes
#include <blons/graphics/gui/textarea.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Identical to gui::Textarea except for appearance.
///
/// \copydoc gui::Textarea
////////////////////////////////////////////////////////////////////////////////
class ConsoleTextarea : public Textarea
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Textarea(Box, Skin::FontStyle, Manager*, Window*)
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleTextarea(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window)
        : Textarea(pos, style, parent_manager, parent_window) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls ConsoleTextarea(Box, Skin::FontStyle, Manager*, Window*) with a
    /// style of Skin::FontStyle::CONSOLE
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleTextarea(Box pos, Manager* parent_manager, Window* parent_window)
        : ConsoleTextarea(pos, Skin::FontStyle::CONSOLE, parent_manager, parent_window) {}
    ~ConsoleTextarea() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context) override;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::ConsoleTextarea
/// \ingroup gui
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GUI_CONSOLETEXTAREA_H_
