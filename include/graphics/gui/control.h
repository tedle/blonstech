#ifndef BLONSTECH_GRAPHICS_GUI_CONTROL_H_
#define BLONSTECH_GRAPHICS_GUI_CONTROL_H_

// Local Includes
#include "graphics/gui/skin.h"
#include "graphics/render/drawbatcher.h"
#include "input/inputtemp.h"

namespace blons
{
namespace GUI
{
// Forward declarations
class Manager;
class Window;

// Full draw call info for rendering
struct DrawCallInputs
{
    bool is_text;
    FontType usage;
    Vector4 colour;
    Box crop;
    int crop_feather;
};

// Cacheable draw call info
struct StaticDrawCallInputs
{
    bool is_text;
    FontType usage;
    Vector4 colour;
    // needed for efficient std::map lookups
    bool operator< (const StaticDrawCallInputs call) const { return memcmp(this, &call, sizeof(StaticDrawCallInputs))>0; }
};

class Control
{
public:
    virtual void Render(RenderContext& context)=0;
    virtual bool Update(const Input& input)=0;

    void set_pos(float x, float y);
    Box pos() const;

    void set_crop(Box crop, int feather);

protected:
    // Feel kinda dirty doing all this initialization here
    Box pos_ = Box(0, 0, 0, 0);
    Box crop_ = Box(0, 0, 0, 0);
    int feather_ = 0;
    Manager* gui_ = nullptr;
    Window* parent_ = nullptr;

    // Order of draw calls is the same as order of calls to RegisterBatches()!!!
    void RegisterBatches();
    void ClearBatches();

    DrawBatcher* batch(StaticDrawCallInputs inputs, RenderContext& context);
    DrawBatcher* font_batch(FontType usage, Vector4 colour, RenderContext& context);
    DrawBatcher* control_batch(RenderContext& context);

private:
    // Vector + batch index stored to recycle as much memory as we can
    std::vector<std::pair<StaticDrawCallInputs, std::unique_ptr<DrawBatcher>>> draw_batches_;
    std::size_t batch_index_ = 0;
};
} // namespace GUI
} // namespace blons
#endif