#include <graphics/camera.h>

namespace blons
{
Camera::Camera()
{
    pos_ = Vector3(0.0f, 0.0f, 0.0f);
    rot_ = Vector3(0.0f, 0.0f, 0.0f);
}

Camera::~Camera()
{
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

    view_matrix_ = MatrixLookAt(pos_, look, up);
    rotation = Vector3PitchYawRoll(view_matrix_);
    
    set_rot(rotation.x, rotation.y, rotation.z);
}

void Camera::Render()
{
    view_matrix_ = MatrixView(pos_, rot_);

    return;
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
    return view_matrix_;
}
} // namespace blons