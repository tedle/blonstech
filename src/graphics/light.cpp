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

#include <blons/graphics/light.h>

// Public Includes
#include <blons/graphics/camera.h>

namespace blons
{
Light::Light(Type type, Vector3 pos, Vector3 dir, Vector3 colour)
{
    view_.reset(new Camera);
    type_ = type;
    set_colour(colour);
    set_direction(dir);
    if (type_ != DIRECTIONAL)
    {
        set_pos(pos);
    }
    else
    {
        pos_ = Vector3(0, 0, 0);
    }
}

const Vector3& Light::colour() const
{
    return colour_;
}

const Vector3& Light::direction() const
{
    return direction_;
}

const Vector3& Light::pos() const
{
    return pos_;
}

Matrix Light::view_matrix() const
{
    return view_->view_matrix();
}

void Light::set_colour(const Vector3& colour)
{
    colour_ = colour;
}

void Light::set_direction(const Vector3& dir)
{
    direction_ = Vector3Normalize(dir);
    // Set the camera's direction too
    view_->set_pos(0, 0, 0);
    view_->LookAt(dir.x, dir.y, dir.z);
    view_->set_pos(pos_.x, pos_.y, pos_.z);
}

void Light::set_pos(const Vector3& pos)
{
    if (type_ != DIRECTIONAL)
    {
        pos_ = pos;
        // Set the camera's position too
        view_->set_pos(pos.x, pos.y, pos.z);
    }
}
} // namespace blons