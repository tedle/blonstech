#ifndef BLONSTECH_MATH_MATH_H_
#define BLONSTECH_MATH_MATH_H_

// TODO: write own funcs instead of wrapping dxmath
#include <DirectXMath.h>
using namespace DirectX;
#include <string.h>
// Public Includes
#include <blons/math/units.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Contains pixel boundaries in the form of X/Y coordinates and
/// width/height dimensions
////////////////////////////////////////////////////////////////////////////////
struct Box
{
    units::subpixel x, y, w, h;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new box to zero
    ////////////////////////////////////////////////////////////////////////////////
    Box() : x(0), y(0), w(0), h(0) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new box supplied with subpixel resolution coordinates
    ///
    /// \param x X coordinate of the new box
    /// \param y Y coordinate of the new box
    /// \param w Width of the new box
    /// \param h Height of the new box
    ////////////////////////////////////////////////////////////////////////////////
    Box(units::subpixel _x, units::subpixel _y, units::subpixel _w, units::subpixel _h) : x(_x), y(_y), w(_w), h(_h) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new box supplied with pixel coordinates
    ////////////////////////////////////////////////////////////////////////////////
    Box(units::pixel _x, units::pixel _y, units::pixel _w, units::pixel _h) :
        x(units::pixel_to_subpixel(_x)),
        y(units::pixel_to_subpixel(_y)),
        w(units::pixel_to_subpixel(_w)),
        h(units::pixel_to_subpixel(_h)) {}
};

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief 2D vector containing an X and Y coordinate
////////////////////////////////////////////////////////////////////////////////
struct Vector2
{
    units::world x, y;

    Vector2() : x(0), y(0) {}
    Vector2(units::world _x, units::world _y) : x(_x), y(_y) {}

    Vector2& operator= (const Vector2& vec) {x = vec.x; y = vec.y; return *this;}
    Vector2& operator+ (const Vector2& vec) {x += vec.x; y += vec.y; return *this;}
    Vector2& operator- (const Vector2& vec) {x -= vec.x; y -= vec.y; return *this;}
    bool operator== (const Vector2& vec) {return x == vec.x && y == vec.y;}
    bool operator!= (const Vector2& vec) {return !(*this == vec);}
};

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief 3D vector containing XYZ coordinates, or optionally RGB if used for
/// colours
////////////////////////////////////////////////////////////////////////////////
struct Vector3
{
    union
    {
        struct { units::world x, y, z; };
        struct { units::world r, g, b; };
    };

    Vector3() : x(0), y(0), z(0) {}
    Vector3(units::world _x, units::world _y, units::world _z) : x(_x), y(_y), z(_z) {}

    Vector3& operator= (const Vector3& vec) {x = vec.x; y = vec.y; z = vec.z; return *this;}
    Vector3& operator+ (const Vector3& vec) {x += vec.x; y += vec.y; z += vec.z; return *this;}
    Vector3& operator- (const Vector3& vec) {x -= vec.x; y -= vec.y; z -= vec.z; return *this;}
    bool operator== (const Vector3& vec) {return x == vec.x && y == vec.y && z == vec.z;}
    bool operator!= (const Vector3& vec) {return !(*this == vec);}
};

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief 4D vector containing XYZW coordinates, or optionally RGBA if used for
/// colours
////////////////////////////////////////////////////////////////////////////////
struct Vector4
{
    union
    {
        struct { units::world x, y, z, w; };
        struct { units::world r, g, b, a; };
    };

    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(units::world _x, units::world _y, units::world _z, units::world _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vector4(const Box& b) : x(b.x), y(b.y), z(b.w), w(b.h) {}

    Vector4& operator= (const Vector4& vec) {x = vec.x; y = vec.y; z = vec.z; w = vec.w; return *this;}
    Vector4& operator+ (const Vector4& vec) {x += vec.x; y += vec.y; z += vec.z; w = vec.w; return *this;}
    Vector4& operator- (const Vector4& vec) {x -= vec.x; y -= vec.y; z -= vec.z; w = vec.w; return *this;}
    bool operator== (const Vector4& vec) {return x == vec.x && y == vec.y && z == vec.z && w == vec.w;}
    bool operator!= (const Vector4& vec) {return !(*this == vec);}
};

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief 4x4 matrix containing 16 world units for 3D math
////////////////////////////////////////////////////////////////////////////////
struct Matrix
{
    units::world m[4][4];

    // TODO: add overloaded * for muls
    // TODO: get rid of this lmao
    Matrix& operator=(const XMFLOAT4X4 & xm)
    {
        memcpy(this->m, xm.m, sizeof(units::world)*4*4);
        return *this;
    }
    bool operator== (const Matrix& matrix) {return memcmp(m, matrix.m, sizeof(units::world)*4*4) == 0;}
    bool operator!= (const Matrix& matrix) {return !(*this == matrix);}
};

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Generates an identity matrix
///
/// \return New identity matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixIdentity();
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Generates a view matrix pointed at a specific coordinate
///
/// \param pos The position you are looking from
/// \param look The position to look at
/// \param up The rotation info of the view
/// \return New view matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixLookAt(Vector3 pos, Vector3 look, Vector3 up);
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Multiplies 2 matrices and returns the result
///
/// \param a The first matrix
/// \param b The second matrix
/// \return Multiplied matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixMultiply(Matrix a, Matrix b);
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Generates an orthographic projection matrix for 2D rendering
///
/// \param screen_width The width of the screen in subpixel resolution
/// \param screen_height The height of the screen in subpixel resolution
/// \param screen_near The near clipping plane distance from the camera
/// \param screen_depth The far clipping plane distance from the camera
/// \return Orthographic projection matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixOrthographic(units::subpixel screen_width, units::subpixel screen_height,
                          units::world screen_near, units::world screen_depth);
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Generates a perspective projection matrix for 3D rendering
///
/// \param fov The vertical FOV in radians
/// \param screen_aspect The screen width divided by screen height
/// \param screen_near The near clipping plane distance from the camera
/// \param screen_depth The far clipping plane distance from the camera
/// \return Perspective projection matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixPerspectiveFov(float fov, float screen_aspect,
                            units::world screen_near, units::world screen_depth);
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Generates a new matrix located at the supplied coordinates
///
/// \param x X coordinate of the new matrix
/// \param y Y coordinate of the new matrix
/// \param z Z coordinate of the new matrix
/// \return Translated matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixTranslation(float x, float y, float z);
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Transposes the supplied matrix and returns the result
///
/// \param in %Matrix to transpose
/// \return Transposed matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixTranspose(Matrix in);
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Generates a view matrix from a given position and rotation
///
/// \param pos Position of the view matrix
/// \param rot Rotation of the view matrix
/// \return Generated view matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixView(Vector3 pos, Vector3 rot);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Calculates the cross product of 2 given vectors
///
/// \param a The first vector
/// \param b The second vector
/// \return Calculated cross product
////////////////////////////////////////////////////////////////////////////////
Vector3 Vector3Cross(Vector3 a, Vector3 b);
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Normalizes the supplied vector
///
/// \return Normalized vector
////////////////////////////////////////////////////////////////////////////////
Vector3 Vector3Normalize(Vector3 n);
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Retrieves the rotation info from a view matrix
///
/// \return Vector with rotation info where X is pitch, Y is yaw, and Z is roll
////////////////////////////////////////////////////////////////////////////////
Vector3 Vector3PitchYawRoll(Matrix view_matrix);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief pi. its pi
////////////////////////////////////////////////////////////////////////////////
const float kPi = 3.14159265358979323846f;
} // namespace blons

#endif // BLONSTECH_MATH_MATH_H_