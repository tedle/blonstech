#ifndef BLONSTECH_GRAPHICS_GUI_TEXTAREA_H_
#define BLONSTECH_GRAPHICS_GUI_TEXTAREA_H_

// Local Includes
#include "graphics/gui/control.h"

namespace blons
{
namespace GUI
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

    void Render(RenderContext& context);
    bool Update(const Input& input);

    void AddLine(std::string text);
    void Clear();

private:
    FontStyle font_style_;

    void GenLabel(std::string text);
    std::vector<std::string> history_;
    std::vector<std::unique_ptr<Label>> lines_;
    units::pixel line_height_;
};
} // namespace GUI
} // namespace blons
#endif