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

namespace blons
{
Light::Light(Type type, Vector3 pos, Vector3 dir, Vector3 colour)
{
    type_ = type;
    set_colour(colour);
    set_direction(dir);
    set_pos(pos);
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

void Light::set_colour(const Vector3& colour)
{
    colour_ = colour;
}

void Light::set_direction(const Vector3& dir)
{
    direction_ = Vector3Normalize(dir);
}

void Light::set_pos(const Vector3& pos)
{
    pos_ = pos;
}
} // namespace blons