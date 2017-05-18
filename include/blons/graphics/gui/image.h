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

#ifndef BLONSTECH_GRAPHICS_GUI_IMAGE_H_
#define BLONSTECH_GRAPHICS_GUI_IMAGE_H_

// Public Includes
#include <blons/graphics/gui/control.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief UI element that renders an image much like a Sprite.
////////////////////////////////////////////////////////////////////////////////
class Image : public Control
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes members with supplied values. Meant to be used by
    /// gui::Window::MakeImage
    ///
    /// \param pos Position and dimensions of the button
    /// \param filename Filename of the image to be loaded from disk
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    Image(Box pos, std::string filename, Manager* parent_manager, Window* parent_window);
    ////////////////////////////////////////////////////////////////////////////////
    /// \copybrief Image(Box,std::string,Manager*,Window*)
    ///
    /// \param pos Position and dimensions of the button
    /// \param pixel_data PixelData of the image to be loaded
    /// \param parent_manager gui::Manager containing this element
    /// \param parent_window gui::Window containing this element
    ////////////////////////////////////////////////////////////////////////////////
    Image(Box pos, const PixelData& pixel_data, Manager* parent_manager, Window* parent_window);
    ~Image() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ////////////////////////////////////////////////////////////////////////////////
    void Render() override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic which is ignored.
    ///
    /// \param input Handle to the current input events
    /// \return True if the input was consumed by the image
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

private:
    friend Manager;
    // This should be owned by gui::Manager, but a lack of nested forward declarations
    // makes this impossible
    struct InternalHandle
    {
        InternalHandle(std::unique_ptr<Texture> tex, Manager* parent);
        ~InternalHandle();
    private:
        friend Manager;
        std::unique_ptr<Texture> tex_;
        Manager* parent_;
    };
    std::unique_ptr<InternalHandle> image_;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::Image
/// \ingroup gui
///
/// The image element can be created from or attached to any window.
///
/// ### Example:
/// \code
/// // Adding an image to a window
/// auto gui = graphics->gui();
/// auto window = gui->MakeWindow("window_id", 0, 0, 200, 200, "Window title");
/// auto image = window->MakeImage(0, 0, 128, 128, "image.png");
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_IMAGE_H_