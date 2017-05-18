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

#include <blons/graphics/gui/image.h>

// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
Image::Image(Box pos, std::string filename, Manager* parent_manager, Window* parent_window)
    : Control(pos, parent_manager, parent_window)
{
    image_ = gui_->RegisterInternalImage(filename);
}

Image::Image(Box pos, const PixelData& pixel_data, Manager* parent_manager, Window* parent_window)
    : Control(pos, parent_manager, parent_window)
{
    image_ = gui_->RegisterInternalImage(pixel_data);
}

void Image::Render()
{
    Box pos, uv;
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    // Top left corner
    pos.x = x;
    pos.y = y;
    pos.w = pos_.w;
    pos.h = pos_.h;
    uv = Box(0, 0, 1, 1);
    gui_->SubmitImageBatch(pos, uv, crop_, feather_, *image_);
}

bool Image::Update(const Input& input)
{
    return false;
}

Image::InternalHandle::InternalHandle(std::unique_ptr<Texture> tex, Manager* parent)
{
    tex_ = std::move(tex);
    parent_ = parent;
}

Image::InternalHandle::~InternalHandle()
{
    parent_->FreeInternalImage(this);
}
} // namespace gui
} // namespace blons