#ifndef BLONSTECH_GRAPHICS_GUI_LABEL_H_
#define BLONSTECH_GRAPHICS_GUI_LABEL_H_

// Local Includes
#include "graphics/render/render.h"
#include "graphics/gui/colourstring.h"

namespace blons
{
namespace GUI
{
// Forward declarations
class Manager;

class Label
{
public:
    Label(int x, int y, const char* text, Manager* parent_manager);
    ~Label();

    void Render(RenderContext& context);

private:
    Vector2 pos_;
    ColourString text_;

    Manager* gui_;
};
} // namespace GUI
} // namespace blons
#endif