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

#ifndef BLONSTECH_GRAPHICS_GUI_DEBUGSLIDER_H_
#define BLONSTECH_GRAPHICS_GUI_DEBUGSLIDER_H_

// Public Includes
#include <blons/graphics/gui/control.h>

namespace blons
{
namespace gui
{
// Forward declarations
class DebugSliderButton;
class DebugSliderTextbox;

////////////////////////////////////////////////////////////////////////////////
/// \brief Unintuitive UI element for selecting a value within a given range
////////////////////////////////////////////////////////////////////////////////
class DebugSlider : public Control
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes members with supplied values. Meant to be used by
    /// gui::Window::MakeDebugSlider
    ///
    /// \param pos Position and dimensions of the debug slider
    /// \param min Minimum value that can be held by debug slider
    /// \param max Maximum value that can be held by debug slider
    /// \param step Minimum interval that value may be changed by
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    DebugSlider(Box pos, float min, float max, float step, Manager* parent_manager, Window* parent_window);
    ~DebugSlider();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ////////////////////////////////////////////////////////////////////////////////
    void Render() override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic to detect typing and dragging
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the debug slider
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a callback to be invoked when the slider's value is changed.
    /// The callback takes a single argument containing a float of the current
    /// debug slider's value and returns void.
    ///
    /// \param callback Function to be called when value is changed. Accepts a
    /// float containig the debug slider's value and returns void
    ////////////////////////////////////////////////////////////////////////////////
    void set_callback(std::function<void(float)> callback);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a [0,1] representation of the placing between the minimum
    /// and maximum values of the debug slider.
    ///
    /// \return Progress currently held by debug slider
    ////////////////////////////////////////////////////////////////////////////////
    double progress() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a [0,1] representation of the placing between the minimum and
    /// maximum values of the debug slider.
    ///
    /// \param value Value to set
    ////////////////////////////////////////////////////////////////////////////////
    void set_progress(double value);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the value that has been selected with the debug slider
    ///
    /// \return Value currently held by debug slider
    ////////////////////////////////////////////////////////////////////////////////
    float value() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the value inside of the current debug slider.
    ///
    /// \param value Value to set
    ////////////////////////////////////////////////////////////////////////////////
    void set_value(float value);

private:
    float value_;
    float min_, max_, step_;
    double progress_; // [0,1] representation of [min_,max_]
    std::size_t precision_;
    std::function<void(float)> callback_;
    std::unique_ptr<DebugSliderButton> button_;
    std::unique_ptr<DebugSliderTextbox> textbox_;
};
} // namespace gui
} // namespace blons

// TODO: Update documentation when API is settled
////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::DebugSlider
/// \ingroup gui
///
/// The debug slider element can be created from or attached to any window.
/// Supports a user definable callback that is invoked when the adjustable value
/// is changed.
///
/// Used for adjusting debuggable values and not intended for general use due
/// to it's unintuitive design. The textbox can be used to set an exact value.
/// The button can be held and dragged to roughly set a value with increasingly
/// higher precision the more vertical space there is between the cursor and the
/// button.
///
/// ### Example:
/// \code
/// // Adding a debug slider to a window
/// auto gui = graphics->gui();
/// auto window = gui->MakeWindow("window_id", 0, 0, 300, 300, "Window title");
/// auto slider = window->MakeDebugSlider(10, 40, 250, 40, 0.0f, 100.0f, 1.0f);
///
/// // Setting a callback that logs whatever is set
/// slider->set_callback([](float value)
/// {
///     blons::log::Debug("%f\n", value);
/// });
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_DEBUGSLIDER_H_
