#ifndef BLONSTECH_GRAPHICS_CAMERA_H_
#define BLONSTECH_GRAPHICS_CAMERA_H_

#include <blons/math/math.h>

namespace blons
{
class Camera
{
public:
    Camera();
    ~Camera();

    void set_pos(units::world x, units::world y, units::world z);
    void set_rot(units::world pitch, units::world yaw, units::world roll);
    void LookAt(units::world x, units::world y, units::world z);

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