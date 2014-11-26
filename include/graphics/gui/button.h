#ifndef BLONSTECH_GRAPHICS_GUI_BUTTON_H_
#define BLONSTECH_GRAPHICS_GUI_BUTTON_H_

// Includes
#include <functional>
// Local Includes
#include "graphics/gui/control.h"

namespace blons
{
namespace GUI
{
class Button : public Control
{
public:
    Button(int x, int y, int width, int height, const char* label, Manager* parent_manager, Window* parent_window);
    ~Button() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

    void set_callback(std::function<void()> callback);

private:
    Box pos_;
    std::unique_ptr<class Label> label_;
    std::function<void()> callback_;
    bool hover_;
    bool active_;
};
} // namespace GUI
} // namespace blons
#endif