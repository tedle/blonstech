#ifndef BLONSTECH_GRAPHICS_GUI_SKIN_H_
#define BLONSTECH_GRAPHICS_GUI_SKIN_H_

// Includes
#include <memory>
// Local Includes
#include "graphics/render/render.h"

namespace blons
{
// Forward declarations
class Sprite;
namespace GUI
{
class Skin
{
public:
    Skin(RenderContext& context);
    ~Skin();

protected:
    std::unique_ptr<Sprite> skin_;
    // Defined in the header file for all to see
    static const struct
    {
        Box button = Box(0, 0, 100, 100);
        Box window = Box(0, 200, 100, 100);
    } layout_;
};
} // namespace GUI
} // namespace blons
#endif