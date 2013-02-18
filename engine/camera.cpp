#include "camera.h"

Camera::Camera()
{
    pos_.x = 0.0f;
    pos_.y = 0.0f;
    pos_.z = 0.0f;

    rot_.x = 0.0f;
    rot_.y = 0.0f;
    rot_.z = 0.0f;
}

Camera::~Camera()
{
}

void Camera::SetPos(XMFLOAT3 pos)
{
    pos_.x = pos.x;
    pos_.y = pos.y;
    pos_.z = pos.z;

    return;
}

void Camera::SetRot(XMFLOAT3 rot)
{
    rot_.x = rot.x;
    rot_.y = rot.y;
    rot_.z = rot.z;

    return;
}

void Camera::LookAt(XMFLOAT3 look)
{
    XMStoreFloat4x4(&view_matrix_, XMMatrixLookAtLH(XMLoadFloat3(&pos_),
                                                    XMLoadFloat3(&look),
                                                    XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f))));
    float pitch = atan2(-view_matrix_._23,
                        sqrt(view_matrix_._13*view_matrix_._13 +
                             view_matrix_._33*view_matrix_._33));
    float yaw   = atan2(-view_matrix_._31, view_matrix_._11);
    
    SetRot(XMFLOAT3(pitch, yaw, 0.0f));
}

XMFLOAT3 Camera::GetPos()
{
    return pos_;
}

XMFLOAT3 Camera::GetRot()
{
    return rot_;
}

void Camera::Render()
{
    XMFLOAT3 up, pos, look;
    XMVECTOR up_vector, pos_vector, look_vector;
    float yaw, pitch, roll;
    XMMATRIX rot;

    // Up points uhh... yea
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    pos.x = pos_.x;
    pos.y = pos_.y;
    pos.z = pos_.z;

    // Look only has 1.0f on the Z so the Rotation transform doesnt crash, DONT set it to 1 otherwise!!
    look.x = 0.0f;
    look.y = 0.0f;
    look.z = 1.0f;

    // 0.017 = pi/180 , converts to radians BUT WE LIVE IN RADIANS
    pitch = rot_.x; // * 0.0174532925f;
    yaw   = rot_.y; // * 0.0174532925f;
    roll  = rot_.z; // * 0.0174532925f;

    // WTF????
    rot = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // holey crap ms
    look_vector = XMVector3TransformCoord(XMLoadFloat3(&look), rot);
    up_vector   = XMVector3TransformCoord(XMLoadFloat3(&up),   rot);
    pos_vector  = XMLoadFloat3(&pos);

    // Translate look_vector back to possey
    look_vector = pos_vector + look_vector;

    XMStoreFloat4x4(&view_matrix_, XMMatrixLookAtLH(pos_vector, look_vector, up_vector));

    return;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
    return view_matrix_;
}