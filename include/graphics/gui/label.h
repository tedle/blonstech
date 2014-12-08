#ifndef BLONSTECH_GRAPHICS_GUI_LABEL_H_
#define BLONSTECH_GRAPHICS_GUI_LABEL_H_

// Local Includes
#include "graphics/gui/control.h"
#include "graphics/gui/colourstring.h"

namespace blons
{
namespace GUI
{
// Forward declarations
class Manager;

class Label : public Control
{
public:
    Label(Vector2 pos, std::string text, FontStyle style, Manager* parent_manager, Window* parent_window);
    Label(Vector2 pos, std::string text, Manager* parent_manager, Window* parent_window)
        : Label(pos, text, FontStyle::LABEL, parent_manager, parent_window) {}
    ~Label() {}

    void Render(RenderContext& context) override;
    bool Update(const Input& input) override;

    void set_text(std::string text);
    // True if you want $fff colour codes parsed, default true
    void set_colour_parsing(bool colour_parsing);

private:
    FontStyle font_style_;
    ColourString text_;
    bool colour_parsing_;
};
} // namespace GUI
} // namespace blons
#endif