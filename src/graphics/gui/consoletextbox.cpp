#include "graphics/gui/consoletextbox.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace gui
{
void ConsoleTextbox::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    RenderBody(layout->console.textbox, context);
    RenderCursor(layout->console.cursor, context);

    RegisterBatches();

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
} // namespace gui
} // namespace blons
