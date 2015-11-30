////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
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

#ifndef BLONSTECH_GRAPHICS_GUI_DEBUGSLIDERBUTTON_H_
#define BLONSTECH_GRAPHICS_GUI_DEBUGSLIDERBUTTON_H_

// Public Includes
#include <blons/graphics/gui/control.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief UI element that responds to dragging and scales a parent owned value
////////////////////////////////////////////////////////////////////////////////
class DebugSliderButton : public Control
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes members with supplied values. Meant to be used by
    /// gui::DebugSlider
    ///
    /// \param pos Position and dimensions of the button
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    DebugSliderButton(Box pos, Manager* parent_manager, Window* parent_window);
    ~DebugSliderButton() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context) override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic to detect mouse clicks.
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the button
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a callback to be invoked as the button is dragged. Callback
    /// accepts 2 arguments, the first being distance moved horizontally since last
    /// call in pixels, and the second being the cursor's vertical distance from the
    /// button in pixels.
    ///
    /// \param callback Function to be called on drag
    ////////////////////////////////////////////////////////////////////////////////
    void set_callback(std::function<void(units::pixel x_delta, units::pixel y_distance)> callback);

protected:
    void RenderBody(const Skin::Layout::Button& b, RenderContext& context);

private:
    std::function<void(units::pixel x_delta, units::pixel y_distance)> callback_;
    bool hover_;
    bool active_;
};
} // namespace gui
} // namespace blons

#endif // BLONSTECH_GRAPHICS_GUI_DEBUGSLIDERBUTTON_H_