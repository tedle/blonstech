#include "graphics/gui/consolewindow.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
ConsoleWindow::ConsoleWindow(std::string id, Box pos, std::string caption, WindowType type, Manager* parent_manager)
    : Window(id, pos, caption, type, parent_manager)
{
    auto font_style = FontStyle::CONSOLE;
    float textbox_height = 40.0f;
    Box textarea_pos = Box(pos_.x, pos_.y, pos_.w, pos_.h - textbox_height);
    Box textbox_pos = Box(pos_.x, pos_.y + pos_.h - textbox_height, pos_.w, textbox_height);

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

void ConsoleWindow::Render(RenderContext& context)
{
    auto skin = gui_->skin();
    auto layout = skin->layout();
    auto sprite = skin->sprite();
    auto batch = control_batch(context);

    // Render a drop shadow
    auto& shadow = layout->dropshadow.bottom;
    sprite->set_pos(pos_.x,
                    pos_.y + pos_.h,
                    pos_.w,
                    shadow.h);
    sprite->set_subtexture(shadow);
    batch->Append(*sprite->mesh());

    Window::Render(context);
}

bool ConsoleWindow::Update(const Input& input)
{
    return Window::Update(input);
}

void ConsoleWindow::hide()
{
    hidden_ = true;
}

void ConsoleWindow::show()
{
    hidden_ = false;
}
} // namespace GUI
} // namespace blons