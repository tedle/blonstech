#ifndef BLONSTECH_GRAPHICS_GUI_CONTROL_H_
#define BLONSTECH_GRAPHICS_GUI_CONTROL_H_

#include "graphics/render/render.h"
#include "input/inputtemp.h"

namespace blons
{
namespace GUI
{
// Forward declarations
class Manager;

class Control
{
public:
    virtual void Render(RenderContext& context)=0;
    virtual void Update(const Input& input)=0;

protected:
    Box pos_;
    Manager* gui_;
};
}
}
#endif