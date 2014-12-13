#ifndef BLONSTECH_GRAPHICS_GUI_CONTROL_H_
#define BLONSTECH_GRAPHICS_GUI_CONTROL_H_

// Public Includes
#include <blons/math/units.h>
#include <blons/graphics/gui/skin.h>
#include <blons/graphics/render/drawbatcher.h>
#include <blons/input/inputtemp.h>

namespace blons
{
namespace gui
{
// Forward declarations
class Manager;
class Window;

// Full draw call info for rendering
struct DrawCallInputs
{
    bool is_text;
    FontStyle font_style;
    Vector4 colour;
    Box crop;
    units::pixel crop_feather;
};

// Cacheable draw call info
struct StaticDrawCallInputs
{
    bool is_text;
    FontStyle font_style;
    Vector4 colour;
    // needed for efficient std::map lookups
    bool operator< (const StaticDrawCallInputs call) const { return memcmp(this, &call, sizeof(StaticDrawCallInputs))>0; }
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Abstract base class for UI elements
////////////////////////////////////////////////////////////////////////////////
class Control
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes the pos_, gui_, and parent_ members with supplied values
    ////////////////////////////////////////////////////////////////////////////////
    Control(Box pos, Manager* parent_manager, Window* parent_window)
        : pos_(pos), gui_(parent_manager), parent_(parent_window) {}

    virtual void Render(RenderContext& context)=0;
    virtual bool Update(const Input& input)=0;

    virtual void hide();
    virtual void show();
    virtual bool hidden();

    virtual void set_pos(units::subpixel x, units::subpixel y);
    virtual Box pos() const;

    virtual void set_crop(Box crop, units::pixel feather);

protected:
    // Feel kinda dirty doing all this initialization here
    Box pos_ = Box(0, 0, 0, 0);
    Box crop_ = Box(0, 0, 0, 0);
    units::pixel feather_ = 0;
    bool hidden_ = false;
    Manager* gui_ = nullptr;
    Window* parent_ = nullptr;

    // Order of draw calls is the same as order of calls to RegisterBatches()!!!
    void RegisterBatches();
    void ClearBatches();

    DrawBatcher* batch(StaticDrawCallInputs inputs, RenderContext& context);
    DrawBatcher* font_batch(FontStyle style, Vector4 colour, RenderContext& context);
    DrawBatcher* control_batch(RenderContext& context);

private:
    // Vector + batch index stored to recycle as much memory as we can
    std::vector<std::pair<StaticDrawCallInputs, std::unique_ptr<DrawBatcher>>> draw_batches_;
    std::size_t batch_index_ = 0;
};
} // namespace gui
} // namespace blons

#endif // BLONSTECH_GRAPHICS_GUI_CONTROL_H_