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

#ifndef BLONSTECH_GRAPHICS_GUI_DEBUGSLIDERTEXTBOX_H_
#define BLONSTECH_GRAPHICS_GUI_DEBUGSLIDERTEXTBOX_H_

// Public Includes
#include <blons/graphics/gui/textbox.h>

namespace blons
{
namespace gui
{
// Forward delcarations
class Label;

////////////////////////////////////////////////////////////////////////////////
/// \brief UI element for taking typed user input
////////////////////////////////////////////////////////////////////////////////
class DebugSliderTextbox : public Textbox
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes members with supplied values. Meant to be used by
    /// gui::DebugSlider
    ///
    /// \param pos Position and dimensions of the textbox
    /// \param style Type of font to be rendered by the textbox
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    DebugSliderTextbox(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window);
    ~DebugSliderTextbox() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ////////////////////////////////////////////////////////////////////////////////
    void Render() override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic to detect typing and text manipulation
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the textbox
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the percentage progress. Used only during the Render process,
    /// purely aesthetic.
    ///
    /// \param progress double ranging from [0,1] indicating progress of textbox
    ////////////////////////////////////////////////////////////////////////////////
    void set_progress(double progress);

protected:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates mesh for the progress bar and appends it to the textbox's
    /// draw queue
    ////////////////////////////////////////////////////////////////////////////////
    virtual void RenderProgress();

private:
    bool queue_callback_;
    double progress_;
};
} // namespace gui
} // namespace blons

#endif // BLONSTECH_GRAPHICS_GUI_DEBUGSLIDERTEXTBOX_H_