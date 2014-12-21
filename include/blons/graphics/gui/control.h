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

////////////////////////////////////////////////////////////////////////////////
/// \brief Inputs to be sent to the UI shader. Paired one-to-one with each
/// blons::Drawbatcher.
////////////////////////////////////////////////////////////////////////////////
struct DrawCallInputs
{
    bool is_text;
    FontStyle font_style;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Is only applied to text. Values range from 0.0 to 1.0
    ////////////////////////////////////////////////////////////////////////////////
    Vector4 colour;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Only render anything inside this Box. A width of 0 prevents any
    /// horizontal cropping. A height of 0 prevents any vertical cropping.
    ////////////////////////////////////////////////////////////////////////////////
    Box crop;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders a linear alpha fade towards the crop box, approaching 0 at
    /// the boundaries.
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel crop_feather;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Shader inputs that are unlikely to change between frames. Used for
/// caching.
////////////////////////////////////////////////////////////////////////////////
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

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Called once per frame during gui::Manager::Render.
    ///
    /// Make a call to Control::batch, Control::font_batch, or
    /// Control::control_batch and append any mesh data to the resulting
    /// Drawbatcher. Once done make a call to Control::RegisterBatches. The order of
    /// calls to Control::RegisterBatches made by various Control%s is the same as
    /// the order they will appear on screen.
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    virtual void Render(RenderContext& context)=0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Called once per frame during gui::Manager::Update.
    ///
    /// \param input Handle to the current input events
    /// \return True if the input is consumed by the Control. Ends input cycle,
    /// preventing calls to Control::Update on later UI elements for that frame.
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool Update(const Input& input)=0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Hides the Control preventing any calls to Render and Update.
    ////////////////////////////////////////////////////////////////////////////////
    virtual void hide();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Shows the Control allowing all calls to Render and Update.
    ////////////////////////////////////////////////////////////////////////////////
    virtual void show();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Queries whether the Control is hidden currently.
    ///
    /// \return True if the Control is hidden
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool hidden();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the Control%'s position.
    ///
    /// \param x Horizontal position with subpixel resolution
    /// \param y Vertical position with subpixel resolution
    ////////////////////////////////////////////////////////////////////////////////
    virtual void set_pos(units::subpixel x, units::subpixel y);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Queries the position & dimensions of the Control.
    ///
    /// \return Box containing coordinates and dimensions
    ////////////////////////////////////////////////////////////////////////////////
    virtual Box pos() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets boundaries for the Control to be rendered in.
    ///
    /// \param crop Only render anything inside this Box. A width of 0 prevents any
    /// horizontal cropping. A height of 0 prevents any vertical cropping.
    /// \param feather Renders a linear alpha fade towards the crop box, approaching
    /// 0 at the boundaries. A value of 0 prevents feathering.
    ////////////////////////////////////////////////////////////////////////////////
    virtual void set_crop(Box crop, units::pixel feather);

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief The position and dimensions of the Control in subpixel resolution.
    ////////////////////////////////////////////////////////////////////////////////
    Box pos_ = Box(0, 0, 0, 0);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief The boundaries within which the Control must render. A width of 0
    /// means no horizontal boundaries. A height of 0 means no vertical boundaries.
    ////////////////////////////////////////////////////////////////////////////////
    Box crop_ = Box(0, 0, 0, 0);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Renders a linear alpha fade towards the crop box, approaching
    /// 0 at the boundaries. A value of 0 prevents feathering.
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel feather_ = 0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Whether the Control is hidden and receives calls to Render & Update.
    ////////////////////////////////////////////////////////////////////////////////
    bool hidden_ = false;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief The gui::Manager containing this element.
    ////////////////////////////////////////////////////////////////////////////////
    Manager* gui_ = nullptr;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief The gui::Window containing this element.
    ////////////////////////////////////////////////////////////////////////////////
    Window* parent_ = nullptr;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a Drawbatcher with mesh data & shader inputs to the parent
    /// gui::Manager. Order of the calls to this function is the same as the
    /// submitted Drawbatcher%s are rendered onto the screen.
    ////////////////////////////////////////////////////////////////////////////////
    void RegisterBatches();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Clears any queued Drawbatcher%s waiting for submission.
    ////////////////////////////////////////////////////////////////////////////////
    void ClearBatches();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves an unnused Drawbatcher with the specific shader inputs
    /// for a Control to render to.
    ///
    /// \param inputs The shader settings that will be applied to this Drawbatcher
    /// during the render cycle
    /// \param context Handle to the current rendering context
    /// \return The Drawbatcher for rendering
    ////////////////////////////////////////////////////////////////////////////////
    DrawBatcher* batch(StaticDrawCallInputs inputs, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves an unnused Drawbatcher with shader inputs tailored
    /// towards font rendering.
    ///
    /// \param style gui::FontStyle to be drawn
    /// \param colour Colour of the text with values ranging from 0.0 to 1.0
    /// \param context Handle to the current rendering context
    /// \return The Drawbatcher for rendering
    ////////////////////////////////////////////////////////////////////////////////
    DrawBatcher* font_batch(FontStyle style, Vector4 colour, RenderContext& context);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves an unnused Drawbatcher with shader inputs tailored
    /// towards element body rendering.
    ///
    /// \param context Handle to the current rendering context
    /// \return The Drawbatcher for rendering
    ////////////////////////////////////////////////////////////////////////////////
    DrawBatcher* control_batch(RenderContext& context);

private:
    // Vector + batch index stored to recycle as much memory as we can
    std::vector<std::pair<StaticDrawCallInputs, std::unique_ptr<DrawBatcher>>> draw_batches_;
    std::size_t batch_index_ = 0;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Control
/// \ingroup gui
///
/// Elements derived from this class can be created from or attached to any
/// window.
///
/// ### Example:
/// \code
/// // Implementing a new UI element
/// class ClickChecker : public Control
/// {
/// public:
///     ClickChecker(Box pos, Manager* parent_manager, Window* parent_window)
///         : Control(pos, parent_manager, parent_window) {}
///     ~ClickChecker() {}
///
///     void Render(RenderContext& context) override
///     {
///         // Find which part of the user customizable skin we're supposed to render
///         auto layout = gui_->skin()->layout();
///         auto region = layout->button.normal;
///
///         // Sprite containing the skin texture
///         auto sprite = gui_->skin()->sprite();
///
///         // Batch to send mesh data to
///         auto batch = control_batch(context);
///
///         // Follow the containing Window's position as it's dragged around
///         auto parent_pos = parent_->pos();
///         auto x = pos_.x + parent_pos.x;
///         auto y = pos_.y + parent_pos.y;
///
///         // Render the center body of a button
///         sprite->set_pos(x, y, pos_.w, pos_.h);
///         sprite->set_subtexture(region.body);
///         batch->Append(sprite->mesh(), context);
///
///         // Submit the batches to gui::Manager
///         RegisterBatches();
///     }
///
///     bool Update(const Input& input) override
///     {
///         // You can either check for the input state at the end of a frame
///         if (input.IsMouseDown(0))
///         {
///             blons::log::Debug("The left mouse was clicked!\n");
///             return true;
///         }
///
///         // Or parse through input events in the order they happened
///         for (const auto& e : input.event_queue())
///         {
///             if (e.type == Input::Event::MOUSE_DOWN)
///             {
///                 blons::log::Debug("The mouse was clicked!\n");
///                 return true;
///             }
///         }
///         return false;
///     }
/// };
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_CONTROL_H_