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
    Label(Vector2 pos, std::string text, Manager* parent_manager, Window* parent_window);
    Label(Vector2 pos, std::string text, FontType font_type, Manager* parent_manager, Window* parent_window);
    ~Label() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

    void set_text(std::string text);

private:
    void Init(Vector2 pos, std::string text, FontType font_type, Manager* parent_manager, Window* parent_window);

    FontType font_type_;
    ColourString text_;
};
} // namespace GUI
} // namespace blons
#endif