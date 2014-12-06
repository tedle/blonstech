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

    RenderBody(layout->textbox.active, context);
    RenderCursor(layout->textbox.cursor, context);

    RegisterBatches();

    RenderText(context);
}
} // namespace GUI
} // namespace blons
