////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

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
/// \brief Abstract base class for UI elements. Implementation must be able to
/// run on worker threads
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
    /// Make a call to Manager::batch, Manager::font_batch, or
    /// Manager::control_batch and append any mesh data to the resulting
    /// DrawBatcher. The order these functions are called in is the same as the
    /// order they wil appear on screen.
    ////////////////////////////////////////////////////////////////////////////////
    virtual void Render()=0;
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
    /// \brief Sets the Control%'s position and dimensions.
    ///
    /// \param pos Position of Control
    ////////////////////////////////////////////////////////////////////////////////
    virtual void set_pos(Box pos);
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
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Queries the crop position & dimensions of the Control.
    ///
    /// \return Box containing crop coordinates and dimensions
    ////////////////////////////////////////////////////////////////////////////////
    virtual Box crop() const;

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
///     void Render() override
///     {
///         // Find which part of the user customizable skin we're supposed to render
///         auto layout = gui_->skin()->layout();
///         auto region = layout->button.normal;
///
///         // Sprite containing the skin texture
///         auto sprite = gui_->skin()->sprite();
///
///         // Batch to send mesh data to
///         auto batch = gui_->control_batch(crop_, feather_);
///
///         // Follow the containing Window's position as it's dragged around
///         auto parent_pos = parent_->pos();
///         auto x = pos_.x + parent_pos.x;
///         auto y = pos_.y + parent_pos.y;
///
///         // Render the center body of a button
///         sprite->set_pos(x, y, pos_.w, pos_.h);
///         sprite->set_subtexture(region.body);
///         batch->Append(sprite->mesh());
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