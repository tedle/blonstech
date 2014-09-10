#ifndef BLONSTECH_CAMERA_H_
#define BLONSTECH_CAMERA_H_

#include "math.h"

class Camera
{
public:
    Camera();
    ~Camera();

    void set_pos(float x, float y, float z);
    void set_rot(float pitch, float yaw, float roll);
    void LookAt(float x, float y, float z);

    Vector3 pos();
    Vector3 rot();

    void Render();
    Matrix view_matrix();

private:
    Vector3 pos_;
    Vector3 rot_;
    Matrix view_matrix_;
};

#endif