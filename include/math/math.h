#ifndef BLONSTECH_MATH_MATH_H_
#define BLONSTECH_MATH_MATH_H_

// TODO: write own funcs instead of wrapping dxmath
#include <DirectXMath.h>
using namespace DirectX;
#include <string.h>

namespace blons
{
struct Vector2
{
    float x, y;

    Vector2() {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}

    Vector2& operator= (const Vector2& vec) {x = vec.x; y = vec.y; return *this;}
    Vector2& operator+ (const Vector2& vec) {x += vec.x; y += vec.y; return *this;}
    Vector2& operator- (const Vector2& vec) {x -= vec.x; y -= vec.y; return *this;}
    bool operator== (const Vector2& vec) {return x == vec.x && y == vec.y;}
    bool operator!= (const Vector2& vec) {return !(*this == vec);}
};

struct Vector3
{
    float x, y, z;

    Vector3() {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    Vector3& operator= (const Vector3& vec) {x = vec.x; y = vec.y; z = vec.z; return *this;}
    Vector3& operator+ (const Vector3& vec) {x += vec.x; y += vec.y; z += vec.z; return *this;}
    Vector3& operator- (const Vector3& vec) {x -= vec.x; y -= vec.y; z -= vec.z; return *this;}
    bool operator== (const Vector3& vec) {return x == vec.x && y == vec.y && z == vec.z;}
    bool operator!= (const Vector3& vec) {return !(*this == vec);}
};

struct Vector4
{
    float x, y, z, w;

    Vector4() {}
    Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    Vector4& operator= (const Vector4& vec) {x = vec.x; y = vec.y; z = vec.z; w = vec.w; return *this;}
    Vector4& operator+ (const Vector4& vec) {x += vec.x; y += vec.y; z += vec.z; w = vec.w; return *this;}
    Vector4& operator- (const Vector4& vec) {x -= vec.x; y -= vec.y; z -= vec.z; w = vec.w; return *this;}
    bool operator== (const Vector4& vec) {return x == vec.x && y == vec.y && z == vec.z && w == vec.w;}
    bool operator!= (const Vector4& vec) {return !(*this == vec);}
};

struct Matrix
{
    float m[4][4];

    // TODO: add overloaded * for muls
    // TODO: get rid of this lmao
    Matrix& operator=(const XMFLOAT4X4 & xm)
    {
        memcpy(this->m, xm.m, sizeof(float)*4*4);
        return *this;
    }
};

Matrix MatrixIdentity();
Matrix MatrixLookAt(Vector3 pos, Vector3 look, Vector3 up);
Matrix MatrixMultiply(Matrix first, Matrix second);
Matrix MatrixOrthographic(float screen_width, float screen_height,
                          float screen_near, float screen_depth);
Matrix MatrixPerspectiveFov(float fov, float screen_aspect,
                            float screen_near, float screen_depth);
Matrix MatrixTranslation(float x, float y, float z);
Matrix MatrixTranspose(Matrix in);
Matrix MatrixView(Vector3 pos, Vector3 rot);

Vector3 Vector3Cross(Vector3 a, Vector3 b);
Vector3 Vector3Normalize(Vector3 n);
Vector3 Vector3PitchYawRoll(Matrix view_matrix);

const float kPi = 3.14159265358979323846f;
} // namespace blons

#endif