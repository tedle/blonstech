
#ifndef BLONSTECH_GRAPHICS_GUI_CONSOLEWINDOW_H_
#define BLONSTECH_GRAPHICS_GUI_CONSOLEWINDOW_H_

// Local Includes
#include "graphics/gui/control.h"
#include "graphics/gui/window.h"
#include "graphics/gui/consoletextarea.h"
#include "graphics/gui/consoletextbox.h"

namespace blons
{
namespace gui
{
class ConsoleWindow : public Window
{
public:
    ConsoleWindow(std::string id, Box pos, std::string caption, WindowType type, Manager* parent_manager);
    ConsoleWindow(std::string id, Box pos, WindowType type, Manager* parent_manager)
        : ConsoleWindow(id, pos, "", type, parent_manager) {}
    ConsoleWindow(std::string id, Box pos, std::string caption, Manager* parent_manager)
        : ConsoleWindow(id, pos, caption, WindowType::INVISIBLE, parent_manager) {}
    ~ConsoleWindow() {}

    void Render(RenderContext& context) override;
    bool Update(const Input& input) override;

    void hide() override;
    void show() override;

private:
    Animation slide_;
    // TODO: Get rid of hacky state recording
    bool hiding_;
};
} // namepsace GUI
} // namespace blons
#endif
