#ifndef BLONSTECH_GRAPHICS_CAMERA_H_
#define BLONSTECH_GRAPHICS_CAMERA_H_

#include "math/math.h"

namespace blons
{
class Camera
{
public:
    Camera();
    ~Camera();

    void set_pos(float x, float y, float z);
    void set_rot(float pitch, float yaw, float roll);
    void LookAt(float x, float y, float z);

    void Render();

    Vector3 pos() const;
    Vector3 rot() const;
    Matrix view_matrix() const;

private:
    Vector3 pos_;
    Vector3 rot_;
    Matrix view_matrix_;
};
} // namespace blons

#endif