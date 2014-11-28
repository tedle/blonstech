#ifndef BLONSTECH_GRAPHICS_GUI_TEXTBOX_H_
#define BLONSTECH_GRAPHICS_GUI_TEXTBOX_H_

// Local Includes
#include "graphics/gui/control.h"

namespace blons
{
namespace GUI
{
class Textbox : public Control
{
public:
    Textbox(Box pos, Manager* parent_manager, Window* parent_window);
    ~Textbox() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

    void set_callback(std::function<void()> callback);
    std::string text();
    void set_text(std::string text);

private:
    std::string text_;
    std::unique_ptr<class Label> text_label_;
    std::function<void()> callback_;
    bool active_;
};
} // namespace GUI
} // namespace blons
#endif