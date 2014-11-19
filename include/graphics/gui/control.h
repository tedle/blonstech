#ifndef BLONSTECH_GRAPHICS_GUI_CONTROL_H_
#define BLONSTECH_GRAPHICS_GUI_CONTROL_H_

#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
class Control
{
public:
    virtual void Render(RenderContext& context)=0;
    virtual void Update(const Input& input)=0;

protected:
    Box pos_;
};
}
}
#endif