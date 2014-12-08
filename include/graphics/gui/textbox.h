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
    Textbox(Box pos, FontStyle style, Manager* parent_manager, Window* parent_window);
    Textbox(Box pos, Manager* parent_manager, Window* parent_window)
        : Textbox(pos, FontStyle::LABEL, parent_manager, parent_window) {}
    ~Textbox() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

    void set_callback(std::function<void()> callback);
    std::string text() const;
    void set_text(std::string text);

protected:
    void RenderBody(const Skin::Layout::Textbox& t, RenderContext& context);
    void RenderCursor(const Box& cursor, RenderContext& context);
    void RenderText(RenderContext& context);

    // Helper functions
    std::vector<Input::Event> Textbox::GetEventsWithRepeats(const Input& input);
    void OnMouseDown(const Input& input);
    void OnKeyDown(const Input& input, const Input::KeyCode key, Input::Modifiers mods);
    void OnKeyUp(const Input& input, const Input::KeyCode key, Input::Modifiers mods);
    void SetCursorPos(std::string::iterator cursor);
    units::subpixel CursorOffset();

private:
    std::string text_;
    FontStyle font_style_;
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
    // Padding between edge of textbox and text
    units::pixel padding_;
};
} // namespace GUI
} // namespace blons
#endif