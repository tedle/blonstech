#include "camera.h"

Camera::Camera()
{
    pos_ = Vector3(0.0f, 0.0f, 0.0f);
    rot_ = Vector3(0.0f, 0.0f, 0.0f);
}

Camera::~Camera()
{
}

void Camera::SetPos(float x, float y, float z)
{
    pos_ = Vector3(x, y, z);

    return;
}

void Camera::SetRot(float pitch, float yaw, float roll)
{
    rot_ = Vector3(pitch, yaw, roll);

    // Prevent the impossible scenario of the rotational vector overflowing someday
    rot_.x = static_cast<float>(fmod(rot_.x, kPi*100.0));
    rot_.y = static_cast<float>(fmod(rot_.y, kPi*100.0));
    rot_.z = static_cast<float>(fmod(rot_.z, kPi*100.0));

    return;
}

void Camera::LookAt(float x, float y, float z)
{
    Vector3 look = Vector3(x, y, z);
    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 rotation;

    view_matrix_ = MatrixLookAt(pos_, look, up);
    rotation = Vector3PitchYawRoll(view_matrix_);
    
    SetRot(rotation.x, rotation.y, rotation.z);
}

Vector3 Camera::GetPos()
{
    return pos_;
}

Vector3 Camera::GetRot()
{
    return rot_;
}

void Camera::Render()
{
    view_matrix_ = MatrixView(pos_, rot_);

    return;
}

Matrix Camera::GetViewMatrix()
{
    return view_matrix_;
}