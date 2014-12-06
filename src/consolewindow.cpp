#include "graphics/gui/consolewindow.h"

namespace blons
{
namespace GUI
{
ConsoleWindow::ConsoleWindow(std::string id, Box pos, std::string caption, WindowType type, Manager* parent_manager)
    : Window(id, pos, caption, type, parent_manager)
{
    auto font_style = FontStyle::CONSOLE;
    float tbh = 40.0f;
    Box textarea_pos = Box(pos_.x, pos_.y, pos_.w, pos_.h - tbh);
    Box textbox_pos = Box(pos_.x, pos_.y + pos_.h - tbh, pos_.w, tbh);

    auto conarea = std::unique_ptr<ConsoleTextarea>(new ConsoleTextarea(textarea_pos, font_style, parent_manager, this));
    auto conbox = std::unique_ptr<ConsoleTextbox>(new ConsoleTextbox(textbox_pos, font_style, parent_manager, this));

    controls_.push_back(std::move(conarea));
    auto conareaptr = static_cast<ConsoleTextarea*>(controls_.back().get());
    controls_.push_back(std::move(conbox));
    auto conboxptr = static_cast<ConsoleTextbox*>(controls_.back().get());

    conboxptr->set_callback([=]()
    {
        conareaptr->AddLine(conboxptr->text());
        conboxptr->set_text("");
    });
}
}
}