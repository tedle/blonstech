#include "camera.h"

CCamera::CCamera()
{
    m_pos.x = 0.0f;
    m_pos.y = 0.0f;
    m_pos.z = 0.0f;

    m_rot.x = 0.0f;
    m_rot.y = 0.0f;
    m_rot.z = 0.0f;
}

CCamera::~CCamera()
{
}

void CCamera::SetPos(XMFLOAT3 pos)
{
    m_pos.x = pos.x;
    m_pos.y = pos.y;
    m_pos.z = pos.z;

    return;
}

void CCamera::SetRot(XMFLOAT3 rot)
{
    m_rot.x = rot.x;
    m_rot.y = rot.y;
    m_rot.z = rot.z;

    return;
}

XMFLOAT3 CCamera::GetPos()
{
    return m_pos;
}

XMFLOAT3 CCamera::GetRot()
{
    return m_rot;
}

void CCamera::Render()
{
    XMFLOAT3 up, pos, look;
    XMVECTOR upVec, posVec, lookVec;
    float yaw, pitch, roll;
    XMMATRIX rotationMatrix;

    // Up points uhh... yea
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    pos.x = m_pos.x;
    pos.y = m_pos.y;
    pos.z = m_pos.z;

    look.x = 0.0f;
    look.y = 0.0f;
    look.z = 1.0f;

    // 0.017 = pi/180 , converts to radians BUT WE LIVE IN RADIANS
    pitch = m_rot.x;// * 0.0174532925f;
    yaw   = m_rot.y;// * 0.0174532925f;
    roll  = m_rot.z;// * 0.0174532925f;

    // WTF????
    rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // holey crap ms
    lookVec = XMVector3TransformCoord(XMLoadFloat3(&look), rotationMatrix);
    upVec   = XMVector3TransformCoord(XMLoadFloat3(&up),   rotationMatrix);
    posVec  = XMLoadFloat3(&pos);

    // Translate lookVec back to possey
    lookVec = posVec + lookVec;

    m_viewMatrix = DirectX::XMMatrixLookAtLH(posVec, lookVec, upVec);

    return;
}

XMMATRIX CCamera::GetViewMatrix()
{
    return m_viewMatrix;
}