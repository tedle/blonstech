#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <DirectXMath.h>

using namespace DirectX;

class CCamera
{
public:
    CCamera();
    ~CCamera();

    void SetPos(XMFLOAT3);
    void SetRot(XMFLOAT3);

    XMFLOAT3 GetPos();
    XMFLOAT3 GetRot();

    void Render();
    XMMATRIX GetViewMatrix();

private:
    XMFLOAT3 m_pos;
    XMFLOAT3 m_rot;
    XMMATRIX m_viewMatrix;
};

#endif