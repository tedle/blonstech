#include <blons/graphics/gui/consolewindow.h>

// Local Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
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
        console::in(conboxptr->text().c_str());
        conboxptr->set_text("");
    });
    console::RegisterPrintCallback([=](const std::string& s)
    {
        conareaptr->AddText(s);
    });

    controls_.push_back(std::move(conarea));
    controls_.push_back(std::move(conbox));

    hiding_ = false;
    hidden_ = true;
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
    slide_.Update();
    return Window::Update(input);
}

void ConsoleWindow::hide()
{
    if (hiding_)
    {
        show();
        return;
    }
    auto shadow_height = gui_->skin()->layout()->dropshadow.bottom.h;
    Animation::Callback cb = [this, shadow_height](float d)
    {
        pos_.y = -(pos_.h + shadow_height) * d;
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
    auto shadow_height = gui_->skin()->layout()->dropshadow.bottom.h;
    Animation::Callback cb = [this, shadow_height](float d)
    {
        pos_.y = -(pos_.h + shadow_height) * (1.0f - d);
    };
    slide_ = Animation(300, cb, Animation::CUBIC_OUT);
    slide_.Update();
    hidden_ = false;
    hiding_ = false;
}
} // namespace gui
} // namespace blons