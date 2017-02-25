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

#include <blons/graphics/gui/consolewindow.h>

// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
ConsoleWindow::ConsoleWindow(std::string id, Box pos, std::string caption, Type type, Manager* parent_manager)
    : Window(id, pos, caption, type, parent_manager)
{
    auto font_style = Skin::FontStyle::CONSOLE;
    float textbox_height = 40.0f;
    // Attach ConsoleTextbox to the bottom of window with a height of textbox_height
    Box textbox_pos = Box(pos_.x, pos_.y + pos_.h - textbox_height, pos_.w, textbox_height);
    // Have ConsoleTextarea fill all remaining space
    Box textarea_pos = Box(pos_.x, pos_.y, pos_.w, pos_.h - textbox_height);

    auto conarea = std::unique_ptr<ConsoleTextarea>(new ConsoleTextarea(textarea_pos, font_style, parent_manager, this));
    auto conbox = std::unique_ptr<ConsoleTextbox>(new ConsoleTextbox(textbox_pos, font_style, parent_manager, this));

    auto conareaptr = conarea.get();
    auto conboxptr = conbox.get();
    // Have ConsoleTextbox feed all input to game console when return is pressed
    conbox->set_callback([](Textbox* textbox)
    {
        console::in(textbox->text().c_str());
        textbox->set_text("");
    });
    // Have game console feed all output to ConsoleTextarea
    console::RegisterPrintCallback([=](const std::string& s)
    {
        conareaptr->AddText(s);
    });

    controls_.push_back(std::move(conarea));
    controls_.push_back(std::move(conbox));

    hiding_ = false;
    hidden_ = true;
}

bool ConsoleWindow::Update(const Input& input)
{
    slide_.Update();
    if (!hiding_)
    {
        return Window::Update(input);
    }
    else
    {
        return false;
    }
}

void ConsoleWindow::hide()
{
    if (hiding_)
    {
        show();
        return;
    }
    Animation::Callback cb = [this](float d)
    {
        pos_.y = -pos_.h * d;
        // Animation has finished
        if (d == 1.0)
        {
            hiding_ = false;
            hidden_ = true;
        }
    };
    slide_ = Animation(300, cb, Animation::CUBIC_IN);
    hiding_ = true;
}

void ConsoleWindow::show()
{
    Animation::Callback cb = [this](float d)
    {
        pos_.y = -pos_.h * (1.0f - d);
    };
    slide_ = Animation(300, cb, Animation::CUBIC_OUT);
    slide_.Update();
    hidden_ = false;
    hiding_ = false;
}
} // namespace gui
} // namespace blons