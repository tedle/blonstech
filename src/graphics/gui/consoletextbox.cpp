#include "graphics/gui/consoletextbox.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
void ConsoleTextbox::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    RenderBody(layout->console.textbox, context);
    RenderCursor(layout->console.cursor, context);

    RegisterBatches();

    RenderText(context);
}
} // namespace GUI
} // namespace blons
