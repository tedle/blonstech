#ifndef BLONSTECH_GRAPHICS_GUI_TEXTAREA_H_
#define BLONSTECH_GRAPHICS_GUI_TEXTAREA_H_

// Public Includes
#include <blons/graphics/gui/control.h>
#include <blons/math/animation.h>
#include <blons/os/timer.h>

namespace blons
{
namespace gui
{
// Forward declarations
class Label;

// Currently immutable to user
class Textarea : public Control
{
public:
    Textarea(Box pos, FontStyle style, Manager* parent_manager, Window* parent_window);
    Textarea(Box pos, Manager* parent_manager, Window* parent_window)
        : Textarea(pos, FontStyle::LABEL, parent_manager, parent_window) {}
    ~Textarea() {}

    void Render(RenderContext& context) override;
    bool Update(const Input& input) override;

    void AddLine(std::string text);
    void AddText(std::string text);
    void Clear();

protected:
    void RenderBody(const Skin::Layout::Textarea& t, RenderContext& context);
    void RenderText(const Skin::Layout::Textarea& t, RenderContext& context);

private:
    void MoveScrollOffset(units::pixel delta, bool smooth);

    FontStyle font_style_;

    // New lines appear on the top, or bottom
    bool newest_top_;
    void GenLabel(std::string text);
    std::string text_;
    std::vector<std::unique_ptr<Label>> lines_;
    units::pixel line_height_;
    units::pixel padding_;

    // What is rendered
    units::pixel scroll_offset_;
    // For animations (scroll_offset_ approaches scroll_destination_)
    units::pixel scroll_destination_;
    Animation scroll_animation_;
    Timer scroll_timer_;
};
} // namespace gui
} // namespace blons

#endif // BLONSTECH_GRAPHICS_GUI_TEXTAREA_H_