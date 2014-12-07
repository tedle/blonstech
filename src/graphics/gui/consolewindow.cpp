#include "graphics/gui/consolewindow.h"

namespace blons
{
namespace GUI
{
ConsoleWindow::ConsoleWindow(std::string id, Box pos, std::string caption, WindowType type, Manager* parent_manager)
    : Window(id, pos, caption, type, parent_manager)
{
    auto font_style = FontStyle::CONSOLE;
    // "True" height of textbox
    float tbh = 40.0f;
    Box textarea_pos = Box(pos_.x, pos_.y, pos_.w, pos_.h - tbh);
    // Textbox skin height is actually 120px (80 for drop shadow, 40 for textbox)...
    // so we use weird multiplies here to not break things. That's dirty and should change
    Box textbox_pos = Box(pos_.x, pos_.y + pos_.h - tbh * 2, pos_.w, tbh * 3);

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