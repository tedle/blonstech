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

#include <blons/graphics/camera.h>

namespace blons
{
Camera::Camera()
{
}

void Camera::set_exposure(float exposure)
{
    exposure_ = powf(2.0f, exposure);
}

void Camera::set_pos(units::world x, units::world y, units::world z)
{
    pos_ = Vector3(x, y, z);

    return;
}

void Camera::set_rot(units::world pitch, units::world yaw, units::world roll)
{
    rot_ = Vector3(pitch, yaw, roll);

    // Prevent the impossible scenario of the rotational vector overflowing someday
    rot_.x = static_cast<units::world>(fmod(rot_.x, kPi*100.0));
    rot_.y = static_cast<units::world>(fmod(rot_.y, kPi*100.0));
    rot_.z = static_cast<units::world>(fmod(rot_.z, kPi*100.0));

    return;
}

void Camera::LookAt(units::world x, units::world y, units::world z)
{
    Vector3 look = Vector3(x, y, z);
    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 rotation;

    rotation = VectorPitchYawRoll(MatrixLookAt(pos_, look, up));
    
    set_rot(rotation.x, rotation.y, rotation.z);
}

float Camera::exposure() const
{
    return exposure_;
}

Vector3 Camera::pos() const
{
    return pos_;
}

Vector3 Camera::rot() const
{
    return rot_;
}

Matrix Camera::view_matrix() const
{
    return MatrixView(pos_, rot_);
}
} // namespace blons