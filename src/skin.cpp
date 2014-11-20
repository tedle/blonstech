#include "graphics/gui/skin.h"

// Local Includes
#include "graphics/sprite.h"

namespace blons
{
namespace GUI
{
Skin::Skin(RenderContext& context)
{
    skin_ = std::unique_ptr<Sprite>(new Sprite("../../notes/skin.png", context));
}

Skin::~Skin()
{

}
} // namespace GUI
} // namespace blons