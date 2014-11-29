#ifndef BLONSTECH_GRAPHICS_GUI_TEXTBOX_H_
#define BLONSTECH_GRAPHICS_GUI_TEXTBOX_H_

// Local Includes
#include "graphics/gui/control.h"
#include "os/timer.h"

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
    std::string text() const;
    void set_text(std::string text);

private:
    std::string text_;
    std::string::iterator cursor_;
    Timer cursor_blink_;
    struct
    {
        Timer timer;
        Input::KeyCode code;
    } key_repeat_;
    std::unique_ptr<class Label> text_label_;
    std::function<void()> callback_;
    bool active_;

    // Helper functions
    std::vector<Input::Event> Textbox::GetEventsWithRepeats(const Input& input);
    void OnMouseDown(const Input& input);
    void OnKeyDown(const Input& input, const Input::KeyCode key, Input::Modifiers mods);
    void OnKeyUp(const Input& input, const Input::KeyCode key, Input::Modifiers mods);
};
} // namespace GUI
} // namespace blons
#endif