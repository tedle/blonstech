////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef BLONSTECH_MATH_MATH_H_
#define BLONSTECH_MATH_MATH_H_

// TODO: write own funcs instead of wrapping dxmath
#include <DirectXMath.h>
using namespace DirectX;
// Includes
#include <cstddef>
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
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Position and dimensions of the box
    units::subpixel x, y, w, h;
    //@}
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new box to zero
    ////////////////////////////////////////////////////////////////////////////////
    Box() : x(0), y(0), w(0), h(0) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new box supplied with subpixel resolution coordinates
    ///
    /// \param _x X coordinate of the new box
    /// \param _y Y coordinate of the new box
    /// \param _w Width of the new box
    /// \param _h Height of the new box
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
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// World coordinates of the vector
    units::world x, y;
    //@}
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a vector to zero
    ////////////////////////////////////////////////////////////////////////////////
    Vector2() : x(0), y(0) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a vector from supplied world units
    ////////////////////////////////////////////////////////////////////////////////
    Vector2(units::world _x, units::world _y) : x(_x), y(_y) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Assignment operator
    ////////////////////////////////////////////////////////////////////////////////
    Vector2& operator= (const Vector2& vec) { x = vec.x; y = vec.y; return *this; }
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Addition operator
    Vector2& operator+= (const Vector2& vec) { x += vec.x; y += vec.y; return *this; }
    Vector2& operator+= (const units::world& f) { x += f; y += f; return *this; }
    Vector2 operator+ (const Vector2& vec) const { return Vector2(x + vec.x, y + vec.y); }
    Vector2 operator+ (const units::world& f) const { return Vector2(x + f, y + f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Subtraction operator
    Vector2& operator-= (const Vector2& vec) { x -= vec.x; y -= vec.y; return *this; }
    Vector2& operator-= (const units::world& f) { x -= f; y -= f; return *this; }
    Vector2 operator- (const Vector2& vec) const { return Vector2(x - vec.x, y - vec.y); }
    Vector2 operator- (const units::world& f) const { return Vector2(x - f, y - f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Multiplication operator
    Vector2& operator*= (const Vector2& vec) { x *= vec.x; y *= vec.y; return *this; }
    Vector2& operator*= (const units::world& f) { x *= f; y *= f; return *this; }
    Vector2 operator* (const Vector2& vec) const { return Vector2(x * vec.x, y * vec.y); }
    Vector2 operator* (const units::world& f) const { return Vector2(x * f, y * f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Division operator
    Vector2& operator/= (const Vector2& vec) { x /= vec.x; y /= vec.y; return *this; }
    Vector2& operator/= (const units::world& f) { x /= f; y /= f; return *this; }
    Vector2 operator/ (const Vector2& vec) const { return Vector2(x / vec.x, y / vec.y); }
    Vector2 operator/ (const units::world& f) const { return Vector2(x / f, y / f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Equality operator
    ////////////////////////////////////////////////////////////////////////////////
    bool operator== (const Vector2& vec) const { return x == vec.x && y == vec.y; }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Inequality operator
    ////////////////////////////////////////////////////////////////////////////////
    bool operator!= (const Vector2& vec) const { return !(*this == vec); }
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

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a vector to zero
    ////////////////////////////////////////////////////////////////////////////////
    Vector3() : x(0), y(0), z(0) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a vector from supplied world units
    ////////////////////////////////////////////////////////////////////////////////
    Vector3(units::world _x, units::world _y, units::world _z) : x(_x), y(_y), z(_z) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Assignment operator
    ////////////////////////////////////////////////////////////////////////////////
    Vector3& operator= (const Vector3& vec) { x = vec.x; y = vec.y; z = vec.z; return *this; }
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Addition operator
    Vector3& operator+= (const Vector3& vec) { x += vec.x; y += vec.y; z += vec.z; return *this; }
    Vector3& operator+= (const units::world& f) { x += f; y += f; z += f; return *this; }
    Vector3 operator+ (const Vector3& vec) const { return Vector3(x + vec.x, y + vec.y, z + vec.z); }
    Vector3 operator+ (const units::world& f) const { return Vector3(x + f, y + f, z + f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Subtraction operator
    Vector3& operator-= (const Vector3& vec) { x -= vec.x; y -= vec.y; z -= vec.z; return *this; }
    Vector3& operator-= (const units::world& f) { x -= f; y -= f; z -= f; return *this; }
    Vector3 operator- (const Vector3& vec) const { return Vector3(x - vec.x, y - vec.y, z - vec.z); }
    Vector3 operator- (const units::world& f) const { return Vector3(x - f, y - f, z - f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Multiplication operator
    Vector3& operator*= (const Vector3& vec) {x *= vec.x; y *= vec.y; z *= vec.z; return *this;}
    Vector3& operator*= (const units::world& f) {x *= f; y *= f; z *= f; return *this;}
    Vector3 operator* (const Vector3& vec) const { return Vector3(x * vec.x, y * vec.y, z * vec.z); }
    Vector3 operator* (const units::world& f) const { return Vector3(x * f, y * f, z * f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Division operator
    Vector3& operator/= (const Vector3& vec) {x /= vec.x; y /= vec.y; z /= vec.z; return *this;}
    Vector3& operator/= (const units::world& f) {x /= f; y /= f; z /= f; return *this;}
    Vector3 operator/ (const Vector3& vec) const { return Vector3(x / vec.x, y / vec.y, z / vec.z); }
    Vector3 operator/ (const units::world& f) const { return Vector3(x / f, y / f, z / f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Equality operator
    ////////////////////////////////////////////////////////////////////////////////
    bool operator== (const Vector3& vec) const { return x == vec.x && y == vec.y && z == vec.z; }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Inequality operator
    ////////////////////////////////////////////////////////////////////////////////
    bool operator!= (const Vector3& vec) const { return !(*this == vec); }
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

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a vector to zero
    ////////////////////////////////////////////////////////////////////////////////
    Vector4() : x(0), y(0), z(0), w(0) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a vector from supplied world units
    ////////////////////////////////////////////////////////////////////////////////
    Vector4(units::world _x, units::world _y, units::world _z, units::world _w) : x(_x), y(_y), z(_z), w(_w) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a vector from a Box mapping x -> x, y -> y, width -> z,
    /// and height -> w
    ////////////////////////////////////////////////////////////////////////////////
    Vector4(const Box& b) : x(b.x), y(b.y), z(b.w), w(b.h) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Assignment operator
    ////////////////////////////////////////////////////////////////////////////////
    Vector4& operator= (const Vector4& vec) { x = vec.x; y = vec.y; z = vec.z; w = vec.w; return *this; }
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Addition operator
    Vector4& operator+= (const Vector4& vec) { x += vec.x; y += vec.y; z += vec.z; w = vec.w; return *this; }
    Vector4& operator+= (const units::world& f) { x += f; y += f; z += f; w = f; return *this; }
    Vector4 operator+ (const Vector4& vec) const { return Vector4(x + vec.x, y + vec.y, z + vec.z, vec.w); }
    Vector4 operator+ (const units::world& f) const { return Vector4(x + f, y + f, z + f, f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Subtraction operator
    Vector4& operator-= (const Vector4& vec) { x -= vec.x; y -= vec.y; z -= vec.z; w = vec.w; return *this; }
    Vector4& operator-= (const units::world& f) { x -= f; y -= f; z -= f; w = f; return *this; }
    Vector4 operator- (const Vector4& vec) const { return Vector4(x - vec.x, y - vec.y, z - vec.z, vec.w); }
    Vector4 operator- (const units::world& f) const { return Vector4(x - f, y - f, z - f, f); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Multiplication operator
    Vector4& operator*= (const Vector4& vec) { x *= vec.x; y *= vec.y; z *= vec.z; w = vec.w; return *this; }
    Vector4& operator*= (const units::world& f) { x *= f; y *= f; z *= f; return *this; }
    Vector4 operator* (const Vector4& vec) const { return Vector4(x * vec.x, y * vec.y, z * vec.z, vec.w); }
    Vector4 operator* (const units::world& f) const { return Vector4(x * f, y * f, z * f, w); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    //@{
    /// Division operator
    Vector4& operator/= (const Vector4& vec) { x /= vec.x; y /= vec.y; z /= vec.z; w = vec.w; return *this; }
    Vector4& operator/= (const units::world& f) { x /= f; y /= f; z /= f; return *this; }
    Vector4 operator/ (const Vector4& vec) const { return Vector4(x / vec.x, y / vec.y, z / vec.z, vec.w); }
    Vector4 operator/ (const units::world& f) const { return Vector4(x / f, y / f, z / f, w); }
    //@}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Equality operator
    ////////////////////////////////////////////////////////////////////////////////
    bool operator== (const Vector4& vec) const { return x == vec.x && y == vec.y && z == vec.z && w == vec.w; }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Inequality operator
    ////////////////////////////////////////////////////////////////////////////////
    bool operator!= (const Vector4& vec) const { return !(*this == vec); }
};

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief 4x4 matrix containing 16 world units for 3D math
////////////////////////////////////////////////////////////////////////////////
struct Matrix
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Vectors of the matrix
    ////////////////////////////////////////////////////////////////////////////////
    units::world m[4][4];

    // TODO: add overloaded * for muls
    // TODO: get rid of this lmao
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Assignment operator
    ////////////////////////////////////////////////////////////////////////////////
    Matrix& operator=(const XMFLOAT4X4 & xm)
    {
        memcpy(this->m, xm.m, sizeof(units::world)*4*4);
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Equality operator
    ////////////////////////////////////////////////////////////////////////////////
    bool operator== (const Matrix& matrix) { return memcmp(m, matrix.m, sizeof(units::world)*4*4) == 0; }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Inequality operator
    ////////////////////////////////////////////////////////////////////////////////
    bool operator!= (const Matrix& matrix) { return !(*this == matrix); }
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
/// \brief Generates the inverse of a matrix
///
/// \param mat Matrix to invert
/// \return Inverted matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixInverse(Matrix mat);
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
/// \param left The leftmost position to be rendered on screen
/// \param right The rightmost position to be rendered on screen
/// \param bottom The bottommost position to be rendered on screen
/// \param top The topmost position to be rendered on screen
/// \param screen_near The near clipping plane distance from the camera
/// \param screen_depth The far clipping plane distance from the camera
/// \return Orthographic projection matrix
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixOrthographic(units::world left, units::world right, units::world bottom, units::world top,
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
Matrix MatrixTranslation(units::world x, units::world y, units::world z);
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
/// \brief Calculates the dot product of 2 given vectors
///
/// \param a The first vector
/// \param b The second vector
/// \return Calculated dot product
////////////////////////////////////////////////////////////////////////////////
units::world Vector3Dot(Vector3 a, Vector3 b);
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
/// \brief Calculates a vector transformed by a given matrix
///
/// \param v Vector to be transformed
/// \param m Matrix to transform by
/// \return Transformed vector
////////////////////////////////////////////////////////////////////////////////
Vector3 Vector3Transform(Vector3 v, Matrix m);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Holds position, texture coords, and the normal of a single vertex
////////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Position of the vertex in 3D space
    ////////////////////////////////////////////////////////////////////////////////
    Vector3 pos;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Position of the texture coordinate in 2D space
    ////////////////////////////////////////////////////////////////////////////////
    Vector2 tex;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Normalized 3D vector of the surface normal
    ////////////////////////////////////////////////////////////////////////////////
    Vector3 norm;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Normalized 3D vector of the surface tangent
    ////////////////////////////////////////////////////////////////////////////////
    Vector3 tan;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Normalized 3D vector of the surface bitangent
    ////////////////////////////////////////////////////////////////////////////////
    Vector3 bitan;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Makes vertices sortable allowing for efficient std::map lookups
    ///
    /// Uses sizeof(float) * 8 to avoid comparing tangent and bitangent in a very
    /// hacky, but simple way. This will probably turn into a super nasty bug later.
    ////////////////////////////////////////////////////////////////////////////////
    bool operator< (const Vertex vert) const { return memcmp(this, &vert, sizeof(float) * 8) > 0; }
};

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Retrieves the FNV-1a hash of a given block of memory
///
/// \param data Data to be hashed
/// \param size Size of memory in bytes
/// \return 32-bit hash value
////////////////////////////////////////////////////////////////////////////////
unsigned int FastHash(const void* data, std::size_t size);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Retrieves the FNV-1a hash of a given string
///
/// \param str String to be hashed
/// \return 32-bit hash value
////////////////////////////////////////////////////////////////////////////////
unsigned int FastHash(const char* str);

////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief pi. its pi
////////////////////////////////////////////////////////////////////////////////
const float kPi = 3.14159265358979323846f;
} // namespace blons

#endif // BLONSTECH_MATH_MATH_H_