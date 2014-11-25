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
    Label(int x, int y, const char* text, Manager* parent_manager);
    ~Label() {}

    void Render(RenderContext& context);
    bool Update(const Input& input);

private:
    ColourString text_;
};
} // namespace GUI
} // namespace blons
#endif