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

template <typename WindowType>
WindowType* blons::gui::Manager::MakeWindow(units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string caption, Window::Type type)
{
    static_assert(std::is_base_of<blons::gui::Window, WindowType>::value, "blons::gui::Manager::MakeWindow<T> must be provided a subclass of blons::gui::Window");

    Box win_pos(x, y, width, height);
    windows_.push_back(std::make_unique<WindowType>(win_pos, caption, type, this));
    return static_cast<WindowType*>(windows_.back().get());
}

template <typename OverlayType>
OverlayType* blons::gui::Manager::MakeOverlay()
{
    static_assert(std::is_base_of<blons::gui::Overlay, OverlayType>::value, "blons::gui::Manager::MakeOverlay<T> must be provided a subclass of blons::gui::Overlay");

    overlays_.push_back(std::make_unique<OverlayType>(this));
    return static_cast<OverlayType*>(overlays_.back().get());
}