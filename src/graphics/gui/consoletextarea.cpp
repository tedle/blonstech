#include "graphics/gui/consoletextarea.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
void ConsoleTextarea::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    RenderBody(layout->console.textarea, context);
    RegisterBatches();

    RenderText(layout->console.textarea, context);
}
} // namespace GUI
} // namespace blons