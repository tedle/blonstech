#ifndef BLONSTECH_CAMERA_H_
#define BLONSTECH_CAMERA_H_

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
    Camera();
    ~Camera();

    void SetPos(XMFLOAT3);
    void SetRot(XMFLOAT3);
    void LookAt(XMFLOAT3);

    XMFLOAT3 GetPos();
    XMFLOAT3 GetRot();

    void Render();
    XMFLOAT4X4 GetViewMatrix();

private:
    XMFLOAT3   pos_;
    XMFLOAT3   rot_;
    XMFLOAT4X4 view_matrix_;
};

#endif