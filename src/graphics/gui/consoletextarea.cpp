#include <blons/graphics/gui/consoletextarea.h>

// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
void ConsoleTextarea::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    RenderBody(layout->console.textarea, context);
    RegisterBatches();

    RenderText(layout->console.textarea, context);
}
} // namespace gui
} // namespace blons