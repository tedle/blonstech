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

#include <blons/graphics/gui/consoletextbox.h>

// Includes
#include <algorithm>
// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
ConsoleTextbox::ConsoleTextbox(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window)
    : Textbox(pos, style, parent_manager, parent_window)
{
    command_history_index_ = kCommandHistoryIndexNone;
}

void ConsoleTextbox::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    RenderBody(layout->console.textbox, context);
    RenderCursor(layout->console.cursor, context);
    RenderText(context);
}

bool ConsoleTextbox::Update(const Input& input)
{
    auto mods = input.modifiers();
    auto events = Textbox::GetEventsWithRepeats(input);

    for (const auto& e : events)
    {
        auto key = static_cast<Input::KeyCode>(e.value);
        mods.Update(e);

        if (e.type == Input::Event::KEY_DOWN)
        {
            SearchHistory(key);
            Textbox::OnKeyDown(input, key, mods);
        }
        else if (e.type == Input::Event::KEY_UP)
        {
            Textbox::OnKeyUp(input, key, mods);
        }
    }
    // Consume input while shown
    return true;
}

void ConsoleTextbox::SearchHistory(Input::KeyCode key)
{
    if (key == Input::UP)
    {
        const auto& history = blons::console::history();
        command_history_index_ = std::min(command_history_index_ + 1, static_cast<int>(history.size()) - 1);
        if (command_history_index_ >= 0)
        {
            set_text(*(history.rbegin() + command_history_index_));
        }
    }
    else if (key == Input::DOWN)
    {
        const auto& history = blons::console::history();
        command_history_index_ = std::max(command_history_index_ - 1, kCommandHistoryIndexNone);
        if (command_history_index_ >= 0)
        {
            set_text(*(history.rbegin() + command_history_index_));
        }
        else
        {
            set_text("");
        }
    }
    else
    {
        command_history_index_ = kCommandHistoryIndexNone;
    }
}
} // namespace gui
} // namespace blons
