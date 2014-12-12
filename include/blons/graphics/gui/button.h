#ifndef BLONSTECH_GRAPHICS_GUI_BUTTON_H_
#define BLONSTECH_GRAPHICS_GUI_BUTTON_H_

// Public Includes
#include <blons/graphics/gui/control.h>

namespace blons
{
namespace gui
{
class Button : public Control
{
public:
    Button(Box pos, std::string label, Manager* parent_manager, Window* parent_window);
    ~Button() {}

    void Render(RenderContext& context) override;
    bool Update(const Input& input) override;

    void set_callback(std::function<void()> callback);

protected:
    void RenderBody(const Skin::Layout::Button& b, RenderContext& context);

private:
    std::unique_ptr<class Label> label_;
    std::function<void()> callback_;
    bool hover_;
    bool active_;
};
} // namespace gui
} // namespace blons

#endif // BLONSTECH_GRAPHICS_GUI_BUTTON_H_