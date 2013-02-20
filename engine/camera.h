#ifndef BLONSTECH_CAMERA_H_
#define BLONSTECH_CAMERA_H_

#include "math.h"

class Camera
{
public:
    Camera();
    ~Camera();

    void SetPos(float x, float y, float z);
    void SetRot(float pitch, float yaw, float roll);
    void LookAt(float x, float y, float z);

    Vector3 GetPos();
    Vector3 GetRot();

    void Render();
    Matrix GetViewMatrix();

private:
    Vector3 pos_;
    Vector3 rot_;
    Matrix view_matrix_;
};

#endif