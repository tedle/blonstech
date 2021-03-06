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

#ifndef BLONSTECH_GUI_CONSOLETEXTBOX_H_
#define BLONSTECH_GUI_CONSOLETEXTBOX_H_

// Public Includes
#include <blons/graphics/gui/textbox.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Similar to gui::Textbox. Differences include new aesthetics,
/// "always on top" input consumption, Up/Down input history, and console tab
/// completion.
///
/// \copydoc gui::Textbox
////////////////////////////////////////////////////////////////////////////////
class ConsoleTextbox : public Textbox
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Textbox(Box, Skin::FontStyle, Manager*, Window*)
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleTextbox(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls ConsoleTextbox(Box, Skin::FontStyle, Manager*, Window*) with a
    /// style of Skin::FontStyle::CONSOLE
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleTextbox(Box pos, Manager* parent_manager, Window* parent_window)
        : ConsoleTextbox(pos, Skin::FontStyle::CONSOLE, parent_manager, parent_window) {}
    ~ConsoleTextbox() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ////////////////////////////////////////////////////////////////////////////////
    void Render() override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic to deal with typing, history search, and tab
    /// completion.
    ///
    /// \param input Handle to the current input events
    /// \return True as input is always consumed by this class
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

private:
    void RenderCompletionText();
    // Takes a keycode and if its up or down arrow, sets the textbox text to a
    // recent console command
    void SearchHistory(Input::KeyCode key);
    // Takes a keycode and if its a tab key will try to provide tab completion
    void TabCompletion(Input::KeyCode key);

    static const int kCommandHistoryIndexNone = -1;
    // How far weve scrolled up/down cmd history
    int command_history_index_;
    // Currently selected completion option
    int tab_completion_index_;
    // Caches original query while we flip thru tab results
    std::string search_text_;
    // Shows low contrast completion hints
    std::unique_ptr<Label> completion_text_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::ConsoleTextbox
/// \ingroup gui
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GUI_CONSOLETEXTBOX_H_