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

#ifndef BLONSTECH_GRAPHICS_GUI_CONSOLEWINDOW_H_
#define BLONSTECH_GRAPHICS_GUI_CONSOLEWINDOW_H_

// Public Includes
#include <blons/graphics/gui/control.h>
#include <blons/graphics/gui/window.h>
#include <blons/graphics/gui/consoletextarea.h>
#include <blons/graphics/gui/consoletextbox.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Similar to gui::Window. Differences include new aesthetics, an
/// animated hide/show sliding effect, and the automatic insertion of a
/// gui::ConsoleTextarea and gui::ConsoleTextbox during construction.
///
/// \copydoc gui::Window
////////////////////////////////////////////////////////////////////////////////
class ConsoleWindow : public Window
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Window(std::string, Box, std::string, Window::Type, Manager*)
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleWindow(std::string id, Box pos, std::string caption, Type type, Manager* parent_manager);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls
    /// ConsoleWindow(std::string, Box, std::string, Window::Type, Manager*)
    /// with an empty caption
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleWindow(std::string id, Box pos, Type type, Manager* parent_manager)
        : ConsoleWindow(id, pos, "", type, parent_manager) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls
    /// ConsoleWindow(std::string, Box, std::string, Window::Type, Manager*)
    /// with a style of Window::Type::INVISIBLE
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleWindow(std::string id, Box pos, std::string caption, Manager* parent_manager)
        : ConsoleWindow(id, pos, caption, Type::INVISIBLE, parent_manager) {}
    ~ConsoleWindow() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs no input logic of its own, simply forwards input to child
    /// Control%s.
    ///
    /// \param input Handle to the current input events
    /// \return True if input is consumed by Control%s
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Slides the ConsoleWindow off the top of the screen over the course of
    /// 300ms. hidden() will not return true until this animation is complete.
    ////////////////////////////////////////////////////////////////////////////////
    void hide() override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Slides the ConsoleWindow down from the top of the screen over the
    /// course of 300ms.
    ////////////////////////////////////////////////////////////////////////////////
    void show() override;

private:
    Animation slide_;
    // TODO: Get rid of hacky state recording
    bool hiding_;
};
} // namepsace GUI
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::ConsoleWindow
/// \ingroup gui
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_CONSOLEWINDOW_H_
