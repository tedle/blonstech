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

#include <blons/math/math.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// Vector2 Operators
////////////////////////////////////////////////////////////////////////////////
Vector2& Vector2::operator= (const Vector2& vec) { x = vec.x; y = vec.y; return *this; }

Vector2& Vector2::operator+= (const Vector2& vec) { x += vec.x; y += vec.y; return *this; }
Vector2& Vector2::operator+= (const units::world& f) { x += f; y += f; return *this; }
Vector2 Vector2::operator+ (const Vector2& vec) const { return Vector2(x + vec.x, y + vec.y); }
Vector2 Vector2::operator+ (const units::world& f) const { return Vector2(x + f, y + f); }

Vector2& Vector2::operator-= (const Vector2& vec) { x -= vec.x; y -= vec.y; return *this; }
Vector2& Vector2::operator-= (const units::world& f) { x -= f; y -= f; return *this; }
Vector2 Vector2::operator- (const Vector2& vec) const { return Vector2(x - vec.x, y - vec.y); }
Vector2 Vector2::operator- (const units::world& f) const { return Vector2(x - f, y - f); }

Vector2& Vector2::operator*= (const Vector2& vec) { x *= vec.x; y *= vec.y; return *this; }
Vector2& Vector2::operator*= (const units::world& f) { x *= f; y *= f; return *this; }
Vector2 Vector2::operator* (const Vector2& vec) const { return Vector2(x * vec.x, y * vec.y); }
Vector2 Vector2::operator* (const units::world& f) const { return Vector2(x * f, y * f); }

Vector2& Vector2::operator/= (const Vector2& vec) { x /= vec.x; y /= vec.y; return *this; }
Vector2& Vector2::operator/= (const units::world& f) { x /= f; y /= f; return *this; }
Vector2 Vector2::operator/ (const Vector2& vec) const { return Vector2(x / vec.x, y / vec.y); }
Vector2 Vector2::operator/ (const units::world& f) const { return Vector2(x / f, y / f); }

bool Vector2::operator== (const Vector2& vec) const { return x == vec.x && y == vec.y; }
bool Vector2::operator!= (const Vector2& vec) const { return !(*this == vec); }

////////////////////////////////////////////////////////////////////////////////
/// Vector3 Operators
////////////////////////////////////////////////////////////////////////////////
Vector3& Vector3::operator= (const Vector3& vec) { x = vec.x; y = vec.y; z = vec.z; return *this; }

Vector3& Vector3::operator+= (const Vector3& vec) { x += vec.x; y += vec.y; z += vec.z; return *this; }
Vector3& Vector3::operator+= (const units::world& f) { x += f; y += f; z += f; return *this; }
Vector3 Vector3::operator+ (const Vector3& vec) const { return Vector3(x + vec.x, y + vec.y, z + vec.z); }
Vector3 Vector3::operator+ (const units::world& f) const { return Vector3(x + f, y + f, z + f); }

Vector3& Vector3::operator-= (const Vector3& vec) { x -= vec.x; y -= vec.y; z -= vec.z; return *this; }
Vector3& Vector3::operator-= (const units::world& f) { x -= f; y -= f; z -= f; return *this; }
Vector3 Vector3::operator- (const Vector3& vec) const { return Vector3(x - vec.x, y - vec.y, z - vec.z); }
Vector3 Vector3::operator- (const units::world& f) const { return Vector3(x - f, y - f, z - f); }

Vector3& Vector3::operator*= (const Vector3& vec) {x *= vec.x; y *= vec.y; z *= vec.z; return *this;}
Vector3& Vector3::operator*= (const units::world& f) {x *= f; y *= f; z *= f; return *this;}
Vector3& Vector3::operator*= (const Matrix& mat) { Vector3 vec = MatrixTranspose(mat) * (*this); x = vec.x; y = vec.y; z = vec.z; return *this; }
Vector3 Vector3::operator* (const Vector3& vec) const { return Vector3(x * vec.x, y * vec.y, z * vec.z); }
Vector3 Vector3::operator* (const units::world& f) const { return Vector3(x * f, y * f, z * f); }
Vector3 Vector3::operator* (const Matrix& mat) const { return MatrixTranspose(mat) * (*this); }

Vector3& Vector3::operator/= (const Vector3& vec) {x /= vec.x; y /= vec.y; z /= vec.z; return *this;}
Vector3& Vector3::operator/= (const units::world& f) {x /= f; y /= f; z /= f; return *this;}
Vector3 Vector3::operator/ (const Vector3& vec) const { return Vector3(x / vec.x, y / vec.y, z / vec.z); }
Vector3 Vector3::operator/ (const units::world& f) const { return Vector3(x / f, y / f, z / f); }

bool Vector3::operator== (const Vector3& vec) const { return x == vec.x && y == vec.y && z == vec.z; }
bool Vector3::operator!= (const Vector3& vec) const { return !(*this == vec); }

////////////////////////////////////////////////////////////////////////////////
/// Vector4 Operators
////////////////////////////////////////////////////////////////////////////////
Vector4& Vector4::operator= (const Vector4& vec) { x = vec.x; y = vec.y; z = vec.z; w = vec.w; return *this; }

Vector4& Vector4::operator+= (const Vector4& vec) { x += vec.x; y += vec.y; z += vec.z; w = vec.w; return *this; }
Vector4& Vector4::operator+= (const units::world& f) { x += f; y += f; z += f; w = f; return *this; }
Vector4 Vector4::operator+ (const Vector4& vec) const { return Vector4(x + vec.x, y + vec.y, z + vec.z, vec.w); }
Vector4 Vector4::operator+ (const units::world& f) const { return Vector4(x + f, y + f, z + f, f); }

Vector4& Vector4::operator-= (const Vector4& vec) { x -= vec.x; y -= vec.y; z -= vec.z; w = vec.w; return *this; }
Vector4& Vector4::operator-= (const units::world& f) { x -= f; y -= f; z -= f; w = f; return *this; }
Vector4 Vector4::operator- (const Vector4& vec) const { return Vector4(x - vec.x, y - vec.y, z - vec.z, vec.w); }
Vector4 Vector4::operator- (const units::world& f) const { return Vector4(x - f, y - f, z - f, f); }

Vector4& Vector4::operator*= (const Vector4& vec) { x *= vec.x; y *= vec.y; z *= vec.z; w = vec.w; return *this; }
Vector4& Vector4::operator*= (const units::world& f) { x *= f; y *= f; z *= f; return *this; }
Vector4& Vector4::operator*= (const Matrix& mat) { Vector4 vec = MatrixTranspose(mat) * (*this); x = vec.x; y = vec.y; z = vec.z; w = vec.w; return *this; }
Vector4 Vector4::operator* (const Vector4& vec) const { return Vector4(x * vec.x, y * vec.y, z * vec.z, vec.w); }
Vector4 Vector4::operator* (const units::world& f) const { return Vector4(x * f, y * f, z * f, w); }
Vector4 Vector4::operator* (const Matrix& mat) const { return MatrixTranspose(mat) * (*this); }

Vector4& Vector4::operator/= (const Vector4& vec) { x /= vec.x; y /= vec.y; z /= vec.z; w = vec.w; return *this; }
Vector4& Vector4::operator/= (const units::world& f) { x /= f; y /= f; z /= f; return *this; }
Vector4 Vector4::operator/ (const Vector4& vec) const { return Vector4(x / vec.x, y / vec.y, z / vec.z, vec.w); }
Vector4 Vector4::operator/ (const units::world& f) const { return Vector4(x / f, y / f, z / f, w); }

bool Vector4::operator== (const Vector4& vec) const { return x == vec.x && y == vec.y && z == vec.z && w == vec.w; }
bool Vector4::operator!= (const Vector4& vec) const { return !(*this == vec); }

////////////////////////////////////////////////////////////////////////////////
/// Matrix Operators
////////////////////////////////////////////////////////////////////////////////
Matrix& Matrix::operator*= (const Matrix& mat)
{
    Matrix mul;
    XMFLOAT4X4 xm1, xm2, xmret;
    memcpy(xm1.m, this->m, sizeof(float)*4*4);
    memcpy(xm2.m, mat.m, sizeof(float)*4*4);

    XMStoreFloat4x4(&xmret,
                    XMMatrixMultiply(XMLoadFloat4x4(&xm1), XMLoadFloat4x4(&xm2)));
    mul = xmret;

    memcpy(m, mul.m, sizeof(units::world) * 4 * 4);
    return *this;
}

Matrix Matrix::operator* (const Matrix& mat) const
{
    Matrix ret = *this;
    ret *= mat;
    return ret;
}

Vector3 Matrix::operator* (const Vector3& vec) const
{
    Vector4 ret(vec.x, vec.y, vec.z, 1.0f);
    XMFLOAT4X4 xm;
    XMFLOAT4 xv;
    memcpy(xm.m, this->m, sizeof(float) * 4 * 4);
    memcpy(&xv, &ret, sizeof(float) * 4);

    auto xret = XMVector3Transform(XMLoadFloat4(&xv), XMLoadFloat4x4(&xm));
    XMStoreFloat4(&xv, xret);
    memcpy(&ret, &xv, sizeof(float) * 4);

    ret /= ret.w;

    return Vector3(ret.x, ret.y, ret.z);
}

Vector4 Matrix::operator* (const Vector4& vec) const
{
    Vector4 ret;
    XMFLOAT4X4 xm;
    XMFLOAT4 xv;
    memcpy(xm.m, this->m, sizeof(float) * 4 * 4);
    memcpy(&xv, &vec, sizeof(float) * 4);

    auto xret = XMVector3Transform(XMLoadFloat4(&xv), XMLoadFloat4x4(&xm));
    XMStoreFloat4(&xv, xret);
    memcpy(&ret, &xv, sizeof(float) * 4);

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// Generic Functions
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixIdentity()
{
    XMFLOAT4X4 id;
    Matrix mid;
    XMStoreFloat4x4(&id, XMMatrixIdentity());
    mid = id;
    return mid;
}

Matrix MatrixInverse(Matrix mat)
{
    XMFLOAT4X4 xm_mat, xm_ret;
    Matrix inv;
    memcpy(xm_mat.m, mat.m, sizeof(float)*4*4);
    XMStoreFloat4x4(&xm_ret, XMMatrixInverse(nullptr, XMLoadFloat4x4(&xm_mat)));
    inv = xm_ret;
    return inv;
}

Matrix MatrixLookAt(Vector3 pos, Vector3 look, Vector3 up)
{
    XMFLOAT4X4 xm;
    Matrix view_matrix;

    XMFLOAT3 xm_pos, xm_look, xm_up;
    xm_pos.x = pos.x;
    xm_pos.y = pos.y;
    xm_pos.z = pos.z;

    xm_look.x = look.x;
    xm_look.y = look.y;
    xm_look.z = look.z;

    xm_up.x = up.x;
    xm_up.y = up.y;
    xm_up.z = up.z;

    XMStoreFloat4x4(&xm, XMMatrixLookAtRH(XMLoadFloat3(&xm_pos),
                                          XMLoadFloat3(&xm_look),
                                          XMLoadFloat3(&xm_up)));

    view_matrix = xm;
    return view_matrix;
}

Matrix MatrixOrthographic(float left, float right, float bottom, float top,
                          float screen_near, float screen_depth)
{
    Matrix ortho_matrix;
    XMFLOAT4X4 xm;
    XMStoreFloat4x4(&xm, XMMatrixOrthographicOffCenterRH(left, right, bottom, top,
                                                         screen_near, screen_depth));
    ortho_matrix = xm;
    return ortho_matrix;
}

Matrix MatrixPerspective(float fov, float screen_aspect,
                         float screen_near, float screen_depth)
{
    Matrix proj_matrix;
    XMFLOAT4X4 xm;
    XMStoreFloat4x4(&xm, XMMatrixPerspectiveFovRH(fov, screen_aspect,
                                                  screen_near, screen_depth));
    proj_matrix = xm;
    return proj_matrix;
}

Matrix MatrixTranslation(float x, float y, float z)
{
    Matrix trans;
    XMFLOAT4X4 xmt;
    XMStoreFloat4x4(&xmt, XMMatrixTranslation(x, y, z));
    trans = xmt;
    return trans;
}

Matrix MatrixTranspose(Matrix in)
{
    XMFLOAT4X4 xm;
    memcpy(xm.m, in.m, sizeof(float)*4*4);
    XMStoreFloat4x4(&xm, XMMatrixTranspose(XMLoadFloat4x4(&xm)));
    in = xm;

    return in;
}

Matrix MatrixView(Vector3 pos, Vector3 rot)
{
    Matrix view_matrix;

    XMFLOAT3 view_up, view_pos, view_look;
    XMVECTOR up_vector, pos_vector, look_vector;
    float yaw, pitch, roll;
    XMMATRIX rot_matrix;
    XMFLOAT4X4 xm;

    // Up points uhh... yea
    view_up.x = 0.0f;
    view_up.y = 1.0f;
    view_up.z = 0.0f;

    view_pos.x = pos.x;
    view_pos.y = pos.y;
    view_pos.z = pos.z;

    // Default viewing angle (0 pitch/yaw points this way!)
    view_look.x = 0.0f;
    view_look.y = 0.0f;
    view_look.z = 1.0f;

    // 0.017 = pi/180 , converts to radians BUT WE LIVE IN RADIANS
    pitch = rot.x; // * 0.0174532925f;
    yaw   = rot.y; // * 0.0174532925f;
    roll  = rot.z; // * 0.0174532925f;

    // WTF????
    rot_matrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // holey crap ms
    look_vector = XMVector3TransformCoord(XMLoadFloat3(&view_look), rot_matrix);
    up_vector   = XMVector3TransformCoord(XMLoadFloat3(&view_up),   rot_matrix);
    pos_vector  = XMLoadFloat3(&view_pos);

    // Translate look_vector back to possey
    look_vector = pos_vector + look_vector;

    XMStoreFloat4x4(&xm, XMMatrixLookAtRH(pos_vector, look_vector, up_vector));
    view_matrix = xm;
    return view_matrix;
}

Vector3 VectorCross(Vector3 a, Vector3 b)
{
    Vector3 product;
    product.x = a.y * b.z - a.z * b.y;
    product.y = a.z * b.x - a.x * b.z;
    product.z = a.x * b.y - a.y * b.x;
    return product;
}

units::world VectorDot(Vector3 a, Vector3 b)
{
    units::world product;
    product = a.x * b.x + a.y * b.y + a.z * b.z;
    return product;
}

Vector3 VectorNormalize(Vector3 n)
{
    float dist = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    n.x /= dist;
    n.y /= dist;
    n.z /= dist;
    return n;
}

Vector3 VectorPitchYawRoll(Matrix view_matrix)
{
    Vector3 rot;

    XMFLOAT4X4 xm;
    memcpy(xm.m, view_matrix.m, sizeof(float)*4*4);
    float pitch = atan2(xm._23,
                        sqrt(xm._13 * xm._13 +
                             xm._33 * xm._33));
    float yaw = atan2(xm._31, -xm._11);

    rot.x = pitch;
    rot.y = yaw;
    rot.z = 0.0f;
    // TODO: actually calculate roll here

    return rot;
}

unsigned int FastHash(const void* data, std::size_t size)
{
    static const unsigned int kPrime = 16777619;
    static const unsigned int kOffset = 2166136261;
    unsigned int hash = kOffset;
    for (int i = 0; i < size; i++)
    {
        hash ^= reinterpret_cast<const char*>(data)[i];
        hash *= kPrime;
    }
    return hash;
}

unsigned int FastHash(const char* str)
{
    static const unsigned int kPrime = 16777619;
    static const unsigned int kOffset = 2166136261;
    unsigned int hash = kOffset;
    while (*str != '\0')
    {
        hash ^= *str++;
        hash *= kPrime;
    }
    return hash;
}
} // namespace blons