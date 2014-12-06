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

    auto conareaptr = conarea.get();
    auto conboxptr = conbox.get();
    conbox->set_callback([=]()
    {
        conareaptr->AddLine(conboxptr->text());
        conboxptr->set_text("");
    });

    controls_.push_back(std::move(conarea));
    controls_.push_back(std::move(conbox));
}
} // namespace GUI
} // namespace blons