#ifndef BLONSTECH_GUI_CONSOLETEXTAREA_H_
#define BLONSTECH_GUI_CONSOLETEXTAREA_H_

// Local Includes
#include "graphics/gui/textarea.h"

namespace blons
{
namespace gui
{
class ConsoleTextarea : public Textarea
{
public:
    ConsoleTextarea(Box pos, FontStyle style, Manager* parent_manager, Window* parent_window)
        : Textarea(pos, style, parent_manager, parent_window) {}
    ConsoleTextarea(Box pos, Manager* parent_manager, Window* parent_window)
        : ConsoleTextarea(pos, FontStyle::CONSOLE, parent_manager, parent_window) {}
    ~ConsoleTextarea() {}

    void Render(RenderContext& context) override;
};
} // namespace gui
} // namespace blons
#endif
