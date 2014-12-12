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
struct Box
{
    units::subpixel x, y, w, h;

    Box() : x(0), y(0), w(0), h(0) {}
    Box(units::subpixel _x, units::subpixel _y, units::subpixel _w, units::subpixel _h) : x(_x), y(_y), w(_w), h(_h) {}
    Box(units::pixel _x, units::pixel _y, units::pixel _w, units::pixel _h) :
        x(units::pixel_to_subpixel(_x)),
        y(units::pixel_to_subpixel(_y)),
        w(units::pixel_to_subpixel(_w)),
        h(units::pixel_to_subpixel(_h)) {}
};

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

struct Vector3
{
    units::world x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(units::world _x, units::world _y, units::world _z) : x(_x), y(_y), z(_z) {}

    Vector3& operator= (const Vector3& vec) {x = vec.x; y = vec.y; z = vec.z; return *this;}
    Vector3& operator+ (const Vector3& vec) {x += vec.x; y += vec.y; z += vec.z; return *this;}
    Vector3& operator- (const Vector3& vec) {x -= vec.x; y -= vec.y; z -= vec.z; return *this;}
    bool operator== (const Vector3& vec) {return x == vec.x && y == vec.y && z == vec.z;}
    bool operator!= (const Vector3& vec) {return !(*this == vec);}
};

struct Vector4
{
    units::world x, y, z, w;

    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(units::world _x, units::world _y, units::world _z, units::world _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vector4(const Box& b) : x(b.x), y(b.y), z(b.w), w(b.h) {}

    Vector4& operator= (const Vector4& vec) {x = vec.x; y = vec.y; z = vec.z; w = vec.w; return *this;}
    Vector4& operator+ (const Vector4& vec) {x += vec.x; y += vec.y; z += vec.z; w = vec.w; return *this;}
    Vector4& operator- (const Vector4& vec) {x -= vec.x; y -= vec.y; z -= vec.z; w = vec.w; return *this;}
    bool operator== (const Vector4& vec) {return x == vec.x && y == vec.y && z == vec.z && w == vec.w;}
    bool operator!= (const Vector4& vec) {return !(*this == vec);}
};

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

Matrix MatrixIdentity();
Matrix MatrixLookAt(Vector3 pos, Vector3 look, Vector3 up);
Matrix MatrixMultiply(Matrix first, Matrix second);
Matrix MatrixOrthographic(units::subpixel screen_width, units::subpixel screen_height,
                          units::world screen_near, units::world screen_depth);
Matrix MatrixPerspectiveFov(float fov, float screen_aspect,
                            units::world screen_near, units::world screen_depth);
Matrix MatrixTranslation(float x, float y, float z);
Matrix MatrixTranspose(Matrix in);
Matrix MatrixView(Vector3 pos, Vector3 rot);

Vector3 Vector3Cross(Vector3 a, Vector3 b);
Vector3 Vector3Normalize(Vector3 n);
Vector3 Vector3PitchYawRoll(Matrix view_matrix);

const float kPi = 3.14159265358979323846f;
} // namespace blons

#endif // BLONSTECH_MATH_MATH_H_