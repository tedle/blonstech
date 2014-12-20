#ifndef BLONSTECH_GRAPHICS_CAMERA_H_
#define BLONSTECH_GRAPHICS_CAMERA_H_

#include <blons/math/math.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Provides a view from which to render a scene
////////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a camera with zeroed out position and rotation
    ////////////////////////////////////////////////////////////////////////////////
    Camera();
    ~Camera();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the position of the camera in the scene
    ///
    /// \param x The X coordinate that the camera should move to
    /// \param y The Y coordinate that the camera should move to
    /// \param z The Z coordinate that the camera should move to
    ////////////////////////////////////////////////////////////////////////////////
    void set_pos(units::world x, units::world y, units::world z);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the angle of how the camera is pointed in the scene
    ///
    /// \param pitch The up/down view angle in radians
    /// \param yaw The left/right view angle in radians
    /// \param roll The tilted view angle in radians
    ////////////////////////////////////////////////////////////////////////////////
    void set_rot(units::world pitch, units::world yaw, units::world roll);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the angle of the camera to be looking at a specific point in the
    /// scene
    ///
    /// \param x The X coordinate that the camera should look at
    /// \param y The Y coordinate that the camera should look at
    /// \param z The Z coordinate that the camera should look at
    ////////////////////////////////////////////////////////////////////////////////
    void LookAt(units::world x, units::world y, units::world z);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the position of the camera in the scene
    ///
    /// \return The XYZ coordinates of the camera
    ////////////////////////////////////////////////////////////////////////////////
    Vector3 pos() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the view angle of the camera in the scene
    ///
    /// \return The pitch, yaw, and roll of the camera
    ////////////////////////////////////////////////////////////////////////////////
    Vector3 rot() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Computes and returns a view matrix matching the camera's position
    /// and rotation
    ///
    /// \return Computed view matrix
    ////////////////////////////////////////////////////////////////////////////////
    Matrix view_matrix() const;

private:
    Vector3 pos_;
    Vector3 rot_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Camera
/// \ingroup graphics
///
/// A camera is already provided by blons::Graphics, so unless you are
/// implementing your own render pipeline it is likely best to use that.
///
/// ### Example:
/// \code
/// // Moving the camera
/// graphics->camera()->set_pos(10.0f, 10.0f, 0.0f);
///
/// // Look towards the origin
/// graphics->camera()->LookAt(0.0f, 0.0f, 0.0f);
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_CAMERA_H_