#ifndef BLONSTECH_GRAPHICS_GUI_BUTTON_H_
#define BLONSTECH_GRAPHICS_GUI_BUTTON_H_

// Local Includes
#include "graphics/gui/control.h"

namespace blons
{
namespace GUI
{
class Button : public Control
{
public:
    Button(int x, int y, int width, int height, const char* label, Manager* parent_manager);
    ~Button() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

private:
    Box pos_;
    std::unique_ptr<class Label> label_;
};
} // namespace GUI
} // namespace blons
#endif