#include "math.h"

Matrix MatrixIdentity()
{
    XMFLOAT4X4 id;
    Matrix mid;
    XMStoreFloat4x4(&id, XMMatrixIdentity());
    mid = id;
    return mid;
}

Matrix MatrixLookAt(Vector3 pos, Vector3 look, Vector3 up)
{
    XMFLOAT4X4 xm;
    Matrix view_matrix;

    XMFLOAT3 xm_pos, xm_look, xm_up;
    xm_pos.x = pos.x;
    xm_pos.y = pos.y;
    xm_pos.z = pos.z;

    xm_look.x = look.x;
    xm_look.y = look.y;
    xm_look.z = look.z;

    xm_up.x = up.x;
    xm_up.y = up.y;
    xm_up.z = up.z;

    XMStoreFloat4x4(&xm, XMMatrixLookAtLH(XMLoadFloat3(&xm_pos),
                                          XMLoadFloat3(&xm_look),
                                          XMLoadFloat3(&xm_up)));

    view_matrix = xm;
    return view_matrix;
}

Matrix MatrixOrthographic(float screen_width, float screen_height,
                          float screen_near, float screen_depth)
{
    Matrix ortho_matrix;
    XMFLOAT4X4 xm;
    XMStoreFloat4x4(&xm, XMMatrixOrthographicLH(screen_width, screen_height,
                                                screen_near, screen_depth));
    ortho_matrix = xm;
    return ortho_matrix;
}

Matrix MatrixPerspectiveFov(float fov, float screen_aspect,
                            float screen_near, float screen_depth)
{
    Matrix proj_matrix;
    XMFLOAT4X4 xm;
    XMStoreFloat4x4(&xm, XMMatrixPerspectiveFovLH(fov, screen_aspect,
                                                  screen_near, screen_depth));
    proj_matrix = xm;
    return proj_matrix;
}

Matrix MatrixTranspose(Matrix in)
{
    XMFLOAT4X4 xm;
    memcpy(xm.m, in.m, sizeof(float)*4*4);
    XMStoreFloat4x4(&xm, XMMatrixTranspose(XMLoadFloat4x4(&xm)));
    in = xm;

    return in;
}

Matrix MatrixView(Vector3 pos, Vector3 rot)
{

    Matrix view_matrix;

    XMFLOAT3 view_up, view_pos, view_look;
    XMVECTOR up_vector, pos_vector, look_vector;
    float yaw, pitch, roll;
    XMMATRIX rot_matrix;
    XMFLOAT4X4 xm;

    // Up points uhh... yea
    view_up.x = 0.0f;
    view_up.y = 1.0f;
    view_up.z = 0.0f;

    view_pos.x = pos.x;
    view_pos.y = pos.y;
    view_pos.z = pos.z;

    // Look only has 1.0f on the Z so the Rotation transform doesnt crash, DONT set it to 1 otherwise!!
    view_look.x = 0.0f;
    view_look.y = 0.0f;
    view_look.z = 1.0f;

    // 0.017 = pi/180 , converts to radians BUT WE LIVE IN RADIANS
    pitch = rot.x; // * 0.0174532925f;
    yaw   = rot.y; // * 0.0174532925f;
    roll  = rot.z; // * 0.0174532925f;

    // WTF????
    rot_matrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // holey crap ms
    look_vector = XMVector3TransformCoord(XMLoadFloat3(&view_look), rot_matrix);
    up_vector   = XMVector3TransformCoord(XMLoadFloat3(&view_up),   rot_matrix);
    pos_vector  = XMLoadFloat3(&view_pos);

    // Translate look_vector back to possey
    look_vector = pos_vector + look_vector;

    XMStoreFloat4x4(&xm, XMMatrixLookAtLH(pos_vector, look_vector, up_vector));
    view_matrix = xm;
    return view_matrix;
}

Vector3 Vector3PitchYawRoll(Matrix view_matrix)
{
    Vector3 rot;

    XMFLOAT4X4 xm;
    memcpy(xm.m, view_matrix.m, sizeof(float)*4*4);
    float pitch = atan2(-xm._23,
                        sqrt(xm._13*xm._13 +
                             xm._33*xm._33));
    float yaw   = atan2(-xm._31, xm._11);

    rot.x = pitch;
    rot.y = yaw;
    rot.z = 0.0f;
    // TODO: actually calculate roll here

    return rot;
}