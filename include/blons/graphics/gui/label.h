#ifndef BLONSTECH_GRAPHICS_GUI_LABEL_H_
#define BLONSTECH_GRAPHICS_GUI_LABEL_H_

// Public Includes
#include <blons/graphics/gui/control.h>
#include <blons/graphics/gui/colourstring.h>

namespace blons
{
namespace gui
{
// Forward declarations
class Manager;

class Label : public Control
{
public:
    Label(Vector2 pos, ColourString text, FontStyle style, Manager* parent_manager, Window* parent_window);
    Label(Vector2 pos, ColourString text, Manager* parent_manager, Window* parent_window)
        : Label(pos, text, FontStyle::LABEL, parent_manager, parent_window) {}
    ~Label() {}

    void Render(RenderContext& context) override;
    bool Update(const Input& input) override;

    void set_text(std::string text);
    void set_text(ColourString text);
    void set_text_colour(Vector4 colour);
    const ColourString& text() const;
    // True if you want $fff colour codes parsed, default true
    void set_colour_parsing(bool colour_parsing);

private:
    FontStyle font_style_;
    ColourString text_;
    bool colour_parsing_;
};
} // namespace gui
} // namespace blons

#endif // BLONSTECH_GRAPHICS_GUI_LABEL_H_