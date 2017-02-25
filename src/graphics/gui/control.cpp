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

#include <blons/graphics/gui/control.h>

// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
void Control::hide()
{
    hidden_ = true;
}

void Control::show()
{
    hidden_ = false;
}

bool Control::hidden()
{
    return hidden_;
}

void Control::set_pos(units::subpixel x, units::subpixel y)
{
    pos_.x = x;
    pos_.y = y;
}

Box Control::pos() const
{
    return pos_;
}

void Control::set_crop(Box crop, units::pixel feather)
{
    crop_ = crop;
    feather_ = feather;
}

Box Control::crop() const
{
    return crop_;
}
} // namespace gui
} // namespace blons