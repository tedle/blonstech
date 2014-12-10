#ifndef BLONSTECH_GUI_CONSOLETEXTBOX_H_
#define BLONSTECH_GUI_CONSOLETEXTBOX_H_

// Local Includes
#include <graphics/gui/textbox.h>

namespace blons
{
namespace gui
{
class ConsoleTextbox : public Textbox
{
public:
    ConsoleTextbox(Box pos, FontStyle style, Manager* parent_manager, Window* parent_window)
        : Textbox(pos, style, parent_manager, parent_window) {}
    ConsoleTextbox(Box pos, Manager* parent_manager, Window* parent_window)
        : ConsoleTextbox(pos, FontStyle::CONSOLE, parent_manager, parent_window) {}
    ~ConsoleTextbox() {}

    void Render(RenderContext& context) override;
    bool Update(const Input& input) override;
};
} // namespace gui
} // namespace blons
#endif