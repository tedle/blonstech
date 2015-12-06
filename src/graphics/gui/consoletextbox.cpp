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
    tab_completion_index_ = 0;

    auto colour = gui_->skin()->layout()->console.completion_colour;
    completion_text_.reset(new Label(Vector2(0, 0), ColourString("", colour), font_style(), gui_, parent_));
}

void ConsoleTextbox::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    RenderBody(layout->console.textbox, context);
    RenderCursor(layout->console.cursor, context);
    RenderText(context);
    RenderCompletionText(context);
}

void ConsoleTextbox::RenderCompletionText(RenderContext& context)
{
    const auto text_label = label();
    const auto text_pos = text_label->pos();
    const auto font = gui_->skin()->font(font_style());

    // Grab the same crop settings as base class used for main label
    completion_text_->set_crop(text_label->crop(), padding() / 2);

    // Append completion label to main label
    auto x = text_pos.x + font->string_width(text_label->text().raw_str(), false);
    completion_text_->set_pos(x, text_pos.y);

    completion_text_->Render(context);
}

bool ConsoleTextbox::Update(const Input& input)
{
    auto mods = input.modifiers();
    auto events = Textbox::GetEventsWithRepeats(input);

    UpdateHighlightScroll(input);

    for (const auto& e : events)
    {
        auto key = static_cast<Input::KeyCode>(e.value);
        mods.Update(e);

        if (e.type == Input::Event::MOUSE_DOWN)
        {
            OnMouseDown(input);
        }
        else if (e.type == Input::Event::MOUSE_UP)
        {
            OnMouseUp(input);
        }
        else if (e.type == Input::Event::MOUSE_MOVE_X ||
                 e.type == Input::Event::MOUSE_MOVE_Y)
        {
            OnMouseMove(input);
        }

        if (e.type == Input::Event::KEY_DOWN)
        {
            Textbox::OnKeyDown(input, key, mods);
            SearchHistory(key);
            TabCompletion(key);
        }
        else if (e.type == Input::Event::KEY_UP)
        {
            Textbox::OnKeyUp(input, key, mods);
        }
    }

    // Consume input while shown
    // Ideally we would ignore mouse input to prevent focus ever being lost,
    // but then we lose out on draggable highlighting
    if (!focus())
    {
        set_focus(true);
    }
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

void ConsoleTextbox::TabCompletion(Input::KeyCode key)
{
    const auto& funcs = blons::console::functions();
    std::vector<std::string> matching_funcs;

    // Reset cached search text whenever we aren't tabbing thru it
    if (key != Input::TAB)
    {
        search_text_ = text();
        tab_completion_index_ = 0;
        completion_text_->set_text("");
    }

    // Don't complete an empty string
    if (search_text_.length() == 0)
    {
        return;
    }

    // Find all functions matching search text
    for (const auto& func : funcs)
    {
        bool match = true;
        for (int i = 0; i < search_text_.length(); i++)
        {
            if (i >= func.length() ||
                search_text_[i] != func[i])
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            matching_funcs.push_back(func);
        }
    }

    if (key == Input::TAB)
    {
        std::string current_func;
        // Check if theres a function to complete to
        if (matching_funcs.size() > tab_completion_index_)
        {
            current_func = matching_funcs[tab_completion_index_];
        }
        // Check if we need to set our text to the function or page to the next match
        if (current_func == text())
        {
            tab_completion_index_ = (tab_completion_index_ + 1) % matching_funcs.size();
            current_func = matching_funcs[tab_completion_index_];
        }
        // Update the textbox
        set_text(current_func);
    }

    // Build completion hint string
    std::string hint_text;
    if (matching_funcs.size() > 0)
    {
        if (matching_funcs.size() > tab_completion_index_)
        {
            hint_text = matching_funcs[tab_completion_index_].substr(text().length());
        }
        hint_text += " ... [";
        hint_text += std::to_string(tab_completion_index_ + 1);
        hint_text += "/";
        hint_text += std::to_string(matching_funcs.size());
        hint_text += "]";
    }

    completion_text_->set_text(hint_text);
}
} // namespace gui
} // namespace blons
