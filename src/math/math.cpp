////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
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

// Includes
#include <cmath>

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
Vector3::Vector3(Vector4 v) : Vector3(v.x / v.w, v.y / v.w, v.z / v.w) {}
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
Vector3& Vector3::operator*= (const Matrix& mat) { Vector3 vec = (*this) * mat; x = vec.x; y = vec.y; z = vec.z; return *this; }
Vector3 Vector3::operator* (const Vector3& vec) const { return Vector3(x * vec.x, y * vec.y, z * vec.z); }
Vector3 Vector3::operator* (const units::world& f) const { return Vector3(x * f, y * f, z * f); }

Vector3& Vector3::operator/= (const Vector3& vec) {x /= vec.x; y /= vec.y; z /= vec.z; return *this;}
Vector3& Vector3::operator/= (const units::world& f) {x /= f; y /= f; z /= f; return *this;}
Vector3 Vector3::operator/ (const Vector3& vec) const { return Vector3(x / vec.x, y / vec.y, z / vec.z); }
Vector3 Vector3::operator/ (const units::world& f) const { return Vector3(x / f, y / f, z / f); }

bool Vector3::operator== (const Vector3& vec) const { return x == vec.x && y == vec.y && z == vec.z; }
bool Vector3::operator!= (const Vector3& vec) const { return !(*this == vec); }

////////////////////////////////////////////////////////////////////////////////
/// Vector4 Operators
////////////////////////////////////////////////////////////////////////////////
Vector4::Vector4(Vector3 v) : Vector4(v.x, v.y, v.z, 1.0f) {}
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
Vector4& Vector4::operator*= (const Matrix& mat) { Vector4 vec = (*this) * mat; x = vec.x; y = vec.y; z = vec.z; w = vec.w; return *this; }
Vector4 Vector4::operator* (const Vector4& vec) const { return Vector4(x * vec.x, y * vec.y, z * vec.z, vec.w); }
Vector4 Vector4::operator* (const units::world& f) const { return Vector4(x * f, y * f, z * f, w); }

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

Vector3 Vector3::operator* (const Matrix& mat) const
{
    Vector4 ret(this->x, this->y, this->z, 1.0f);
    XMFLOAT4X4 xm;
    XMFLOAT4 xv;
    memcpy(xm.m, mat.m, sizeof(float) * 4 * 4);
    memcpy(&xv, &ret, sizeof(float) * 4);

    auto xret = XMVector3Transform(XMLoadFloat4(&xv), XMLoadFloat4x4(&xm));
    XMStoreFloat4(&xv, xret);
    memcpy(&ret, &xv, sizeof(float) * 4);

    ret /= ret.w;

    return Vector3(ret.x, ret.y, ret.z);
}

Vector4 Vector4::operator* (const Matrix& mat) const
{
    Vector4 ret;
    XMFLOAT4X4 xm;
    XMFLOAT4 xv;
    memcpy(xm.m, mat.m, sizeof(float) * 4 * 4);
    memcpy(&xv, this, sizeof(float) * 4);

    auto xret = XMVector4Transform(XMLoadFloat4(&xv), XMLoadFloat4x4(&xm));
    XMStoreFloat4(&xv, xret);
    memcpy(&ret, &xv, sizeof(float) * 4);

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// Generic Functions
////////////////////////////////////////////////////////////////////////////////
Matrix MatrixIdentity()
{
    Matrix id;
    id.m[0][0] = 1.0f;
    id.m[1][1] = 1.0f;
    id.m[2][2] = 1.0f;
    id.m[3][3] = 1.0f;
    return id;
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

Matrix MatrixOrthographic(units::world left, units::world right, units::world bottom, units::world top,
                          units::world screen_near, units::world screen_depth)
{
    Matrix ortho_matrix;
    XMFLOAT4X4 xm;
    XMStoreFloat4x4(&xm, XMMatrixOrthographicOffCenterRH(left, right, bottom, top,
                                                         screen_near, screen_depth));
    ortho_matrix = xm;
    return ortho_matrix;
}

Matrix MatrixPerspective(float fov, float screen_aspect,
                         units::world screen_near, units::world screen_far,
                         bool zero_to_one)
{
    Matrix proj_matrix;
    XMFLOAT4X4 xm;
    XMStoreFloat4x4(&xm, XMMatrixPerspectiveFovRH(fov, screen_aspect,
                                                  screen_near, screen_far));
    proj_matrix = xm;
    // DirectXMath assumes a NDC depth buffer range of [0,1], but OpenGL uses [-1,1]
    if (!zero_to_one)
    {
        proj_matrix = proj_matrix * MatrixScale(1.0f, 1.0f, 2.0f) * MatrixTranslation(0.0f, 0.0f, -1.0f);
    }
    return proj_matrix;
}

Matrix MatrixScale(units::world x, units::world y, units::world z)
{
    Matrix scale;
    scale.m[0][0] = x;
    scale.m[1][1] = y;
    scale.m[2][2] = z;
    scale.m[3][3] = 1.0f;
    return scale;
}

Matrix MatrixTranslation(units::world x, units::world y, units::world z)
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
    view_look.z = -1.0f;

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

Vector3 VectorAbsolute(Vector3 v)
{
    return Vector3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

Vector3 VectorCross(Vector3 a, Vector3 b)
{
    Vector3 product;
    product.x = a.y * b.z - a.z * b.y;
    product.y = a.z * b.x - a.x * b.z;
    product.z = a.x * b.y - a.y * b.x;
    return product;
}

units::world VectorDistance(const Vector3& a, const Vector3& b)
{
    return VectorLength(a - b);
}

units::world VectorDot(Vector3 a, Vector3 b)
{
    units::world product;
    product = a.x * b.x + a.y * b.y + a.z * b.z;
    return product;
}

units::world VectorLength(Vector3 v)
{
    units::world length;
    length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return length;
}

Vector3 VectorNormalize(Vector3 n)
{
    float dist = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
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
    float pitch = atan2f(xm._23,
                         sqrtf(xm._13 * xm._13 +
                               xm._33 * xm._33));
    float yaw = atan2f(xm._31, -xm._11);

    rot.x = pitch;
    rot.y = yaw;
    rot.z = 0.0f;
    // TODO: actually calculate roll here

    return rot;
}

namespace
{
// Pre-computed basis functions
inline units::world sh_l0m0()
{
    // {l=0,m=0} = 1 / (2 * sqrt(pi))
    return 0.28209479177387814347403972578039f;
}
inline units::world sh_l1m_1(const Vector3& v)
{
    // {l=1,m=-1} = -(sqrt(3) * y) / (2 * sqrt(pi))
    return -0.48860251190291992158638462283835f * v.y;
}
inline units::world sh_l1m0(const Vector3& v)
{
    // {l=1,m=0} = (sqrt(3) * z) / (2 * sqrt(pi))
    return 0.48860251190291992158638462283835f * v.z;
}
inline units::world sh_l1m1(const Vector3& v)
{
    // {l=1,m=1} = -(sqrt(3) * x) / (2 * sqrt(pi))
    return -0.48860251190291992158638462283835f * v.x;
}
inline units::world sh_l2m_2(const Vector3& v)
{
    // {l=2,m=-2} = (sqrt(15) * y * x) / (2 * sqrt(pi))
    return 1.0925484305920790705433857058027f * v.y * v.x;
}
inline units::world sh_l2m_1(const Vector3& v)
{
    // {l=2,m=-1} = -(sqrt(15) * y * z) / (2 * sqrt(pi))
    return -1.0925484305920790705433857058027f * v.y * v.z;
}
inline units::world sh_l2m0(const Vector3& v)
{
    // {l=2,m=0} = (sqrt(5) * (3 * z^2 - 1)) / (4 * sqrt(pi))
    return 0.94617469575756001809268107088713f * v.z * v.z - 0.31539156525252000603089369029571f;
}
inline units::world sh_l2m1(const Vector3& v)
{
    // {l=2,m=1} = -(sqrt(15) * x * z) / (2 * sqrt(pi))
    return -1.0925484305920790705433857058027f * v.x * v.z;
}
inline units::world sh_l2m2(const Vector3& v)
{
    // {l=2,m=2} = (sqrt(15) * (x^2 - y^2)) / (4 * sqrt(pi))
    return 0.54627421529603953527169285290134f * (v.x * v.x - v.y * v.y);
}
} // namespace

////////////////////////////////////////////////////////////////////////////////
/// SHCoeffs2 Operators
////////////////////////////////////////////////////////////////////////////////
SHCoeffs2& SHCoeffs2::operator= (const SHCoeffs2& shc) { for (int i = 0; i < 4; i++) coeffs[i] = shc.coeffs[i]; return *this; }

SHCoeffs2& SHCoeffs2::operator+= (const SHCoeffs2& shc) { for (int i = 0; i < 4; i++) coeffs[i] += shc.coeffs[i]; return *this; }
SHCoeffs2 SHCoeffs2::operator+ (const SHCoeffs2& shc) const { SHCoeffs2 val; for (int i = 0; i < 4; i++) val.coeffs[i] = coeffs[i] + shc.coeffs[i]; return val; }

SHCoeffs2& SHCoeffs2::operator*= (const SHCoeffs2& shc) { for (int i = 0; i < 4; i++) coeffs[i] *= shc.coeffs[i]; return *this; }
SHCoeffs2& SHCoeffs2::operator*= (const units::world& f) { for (int i = 0; i < 4; i++) coeffs[i] *= f; return *this; }
SHCoeffs2 SHCoeffs2::operator* (const SHCoeffs2& shc) const { SHCoeffs2 val; for (int i = 0; i < 4; i++) val.coeffs[i] = coeffs[i] * shc.coeffs[i]; return val; }
SHCoeffs2 SHCoeffs2::operator* (const units::world& f) const { SHCoeffs2 val; for (int i = 0; i < 4; i++) val.coeffs[i] = coeffs[i] * f; return val; }

SHCoeffs2& SHCoeffs2::operator/= (const SHCoeffs2& shc) { for (int i = 0; i < 4; i++) coeffs[i] /= shc.coeffs[i]; return *this; }
SHCoeffs2& SHCoeffs2::operator/= (const units::world& f) { for (int i = 0; i < 4; i++) coeffs[i] /= f; return *this; }
SHCoeffs2 SHCoeffs2::operator/ (const SHCoeffs2& shc) const { SHCoeffs2 val; for (int i = 0; i < 4; i++) val.coeffs[i] = coeffs[i] / shc.coeffs[i]; return val; }
SHCoeffs2 SHCoeffs2::operator/ (const units::world& f) const { SHCoeffs2 val; for (int i = 0; i < 4; i++) val.coeffs[i] = coeffs[i] / f; return val; }

////////////////////////////////////////////////////////////////////////////////
/// SHCoeffs3 Operators
////////////////////////////////////////////////////////////////////////////////
SHCoeffs3& SHCoeffs3::operator= (const SHCoeffs3& shc) { for (int i = 0; i < 9; i++) coeffs[i] = shc.coeffs[i]; return *this; }

SHCoeffs3& SHCoeffs3::operator+= (const SHCoeffs3& shc) { for (int i = 0; i < 9; i++) coeffs[i] += shc.coeffs[i]; return *this; }
SHCoeffs3 SHCoeffs3::operator+ (const SHCoeffs3& shc) const { SHCoeffs3 val; for (int i = 0; i < 9; i++) val.coeffs[i] = coeffs[i] + shc.coeffs[i]; return val; }

SHCoeffs3& SHCoeffs3::operator*= (const SHCoeffs3& shc) { for (int i = 0; i < 9; i++) coeffs[i] *= shc.coeffs[i]; return *this; }
SHCoeffs3& SHCoeffs3::operator*= (const units::world& f) { for (int i = 0; i < 9; i++) coeffs[i] *= f; return *this; }
SHCoeffs3 SHCoeffs3::operator* (const SHCoeffs3& shc) const { SHCoeffs3 val; for (int i = 0; i < 9; i++) val.coeffs[i] = coeffs[i] * shc.coeffs[i]; return val; }
SHCoeffs3 SHCoeffs3::operator* (const units::world& f) const { SHCoeffs3 val; for (int i = 0; i < 9; i++) val.coeffs[i] = coeffs[i] * f; return val; }

SHCoeffs3& SHCoeffs3::operator/= (const SHCoeffs3& shc) { for (int i = 0; i < 9; i++) coeffs[i] /= shc.coeffs[i]; return *this; }
SHCoeffs3& SHCoeffs3::operator/= (const units::world& f) { for (int i = 0; i < 9; i++) coeffs[i] /= f; return *this; }
SHCoeffs3 SHCoeffs3::operator/ (const SHCoeffs3& shc) const { SHCoeffs3 val; for (int i = 0; i < 9; i++) val.coeffs[i] = coeffs[i] / shc.coeffs[i]; return val; }
SHCoeffs3 SHCoeffs3::operator/ (const units::world& f) const { SHCoeffs3 val; for (int i = 0; i < 9; i++) val.coeffs[i] = coeffs[i] / f; return val; }

SHCoeffs2 SHProjectDirection2(Vector3 direction)
{
    SHCoeffs2 result;
    result.coeffs[0] = sh_l0m0();
    result.coeffs[1] = sh_l1m_1(direction);
    result.coeffs[2] = sh_l1m0(direction);
    result.coeffs[3] = sh_l1m1(direction);
    return result;
}

SHCoeffs3 SHProjectDirection3(Vector3 direction)
{
    SHCoeffs3 result;
    result.coeffs[0] = sh_l0m0();
    result.coeffs[1] = sh_l1m_1(direction);
    result.coeffs[2] = sh_l1m0(direction);
    result.coeffs[3] = sh_l1m1(direction);
    result.coeffs[4] = sh_l2m_2(direction);
    result.coeffs[5] = sh_l2m_1(direction);
    result.coeffs[6] = sh_l2m0(direction);
    result.coeffs[7] = sh_l2m1(direction);
    result.coeffs[8] = sh_l2m2(direction);
    return result;
}

Vector3 TriangleBarycentric(const Triangle& triangle, const Vector3& point)
{
    // Get barycentric coordinates through use of a modified Moller-Trumbore triangle intersection test:
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    // Changes were made to prevent early exit and provide calculation of all barycentric coordinates
    Vector3 barycentric;
    const auto& tri = triangle.vertices;
    Vector3 plane_normal = VectorNormalize(VectorCross(tri[1] - tri[0], tri[2] - tri[0]));
    // Prevents parallel ray intersections by setting the ray origin to be below the hull plane
    Vector3 ray_origin = tri[0] - plane_normal;
    Vector3 ray_dir = point - ray_origin;
    Vector3 edge1 = tri[1] - tri[0];
    Vector3 edge2 = tri[2] - tri[0];

    // There aren't good names for these variables, sorry. They're just re-usable shorthand calculations
    Vector3 P = VectorCross(ray_dir, edge2);
    Vector3 T = ray_origin - tri[0];
    Vector3 Q = VectorCross(T, edge1);
    units::world det = VectorDot(edge1, P);
    if (abs(det) <= std::numeric_limits<units::world>::epsilon())
    {
        throw "Provided point for barycentric triangle calculation is not coplanar";
    }
    units::world inv_det = 1.0f / det;

    barycentric.y = VectorDot(T, P) * inv_det;
    barycentric.z = VectorDot(ray_dir, Q) * inv_det;
    barycentric.x = 1.0f - barycentric.y - barycentric.z;
    return barycentric;
}

Sphere TetrahedronCircumsphere(const Tetrahedron& tetrahedron)
{
    Sphere circumsphere;
    // Circumsphere formula shamelessly stolen from:
    // https://www2.mps.mpg.de/homes/daly/CSDS/t4h/tetra.htm#Q1-1-7
    // Define a tetrahedron where the 1st point lies on the origin
    // d[1,3] being the points that don't lie on the origin
    Vector3 d1 = tetrahedron.vertices[1] - tetrahedron.vertices[0];
    Vector3 d2 = tetrahedron.vertices[2] - tetrahedron.vertices[0];
    Vector3 d3 = tetrahedron.vertices[3] - tetrahedron.vertices[0];
    // Solve for a point (p) that is the center of the new tetrahedron's
    // circumsphere. It can be combined with the original 4th vertex to
    // produce the center
    //     [d1.x d1.y d1.z] [p.x]   [||d1||^2]
    // 2 * [d2.x d2.y d2.z] [p.y] = [||d2||^2]
    //     [d3.x d3.y d3.z] [p.z]   [||d3||^2]
    // Note this is mathematical notation, but our engine uses row-major
    // matrices so we need the transpose instead
    Matrix mat;
    mat.m[0][0] = d1.x; mat.m[0][1] = d2.x; mat.m[0][2] = d3.x; mat.m[0][3] = 0;
    mat.m[1][0] = d1.y; mat.m[1][1] = d2.y; mat.m[1][2] = d3.y; mat.m[1][3] = 0;
    mat.m[2][0] = d1.z; mat.m[2][1] = d2.z; mat.m[2][2] = d3.z; mat.m[2][3] = 0;
    mat.m[3][0] = 0;    mat.m[3][1] = 0;    mat.m[3][2] = 0;    mat.m[3][3] = 1;
    // Solve by multiplying both sides with the inverse of the matrix and 1/2
    Vector3 origin_center(Vector4(VectorDot(d1, d1), VectorDot(d2, d2), VectorDot(d3, d3), 1.0f) * MatrixInverse(mat) * 0.5f);
    // Since one of the vertices is on the origin, taking the length of the center gives us the radius
    circumsphere.radius = VectorLength(origin_center);
    // Combining with the original offset vertex gives us the true center of the circumsphere
    circumsphere.center = tetrahedron.vertices[0] + origin_center;

    return circumsphere;
}

Vector3 SampleAmbientCube(const AmbientCube& cube, Vector3 direction)
{
    AxisAlignedNormal normal_sign[3] = { direction.x > 0.0 ? POSITIVE_X : NEGATIVE_X,
                                         direction.y > 0.0 ? POSITIVE_Y : NEGATIVE_Y,
                                         direction.z > 0.0 ? POSITIVE_Z : NEGATIVE_Z };
    Vector3 direction_sq = direction * direction;
    return cube.coeffs[normal_sign[0]] * direction_sq.x +
           cube.coeffs[normal_sign[1]] * direction_sq.y +
           cube.coeffs[normal_sign[2]] * direction_sq.z;
}

AxisAlignedNormal FindGreatestAxis(Vector3 n)
{
        AxisAlignedNormal direction;
        Vector3 abs_n = VectorAbsolute(n);
        if (abs_n.x > abs_n.y && abs_n.x > abs_n.z)
        {
            if (n.x > 0.0f) { direction = POSITIVE_X; }
            else { direction = NEGATIVE_X; }
        }
        else if (abs_n.y > abs_n.z)
        {
            if (n.y > 0.0f) { direction = POSITIVE_Y; }
            else { direction = NEGATIVE_Y; }
        }
        else
        {
            if (n.z > 0.0f) { direction = POSITIVE_Z; }
            else { direction = NEGATIVE_Z; }
        }
        return direction;
}

Vector3 AxisRotationPitchYawRoll(AxisAlignedNormal direction)
{
    units::world pitch = 0.0f;
    units::world yaw = 0.0f;
    units::world roll = 0.0f;
    switch (direction)
    {
    case NEGATIVE_Z:
        roll = kPi;
        break;
    case POSITIVE_X:
        yaw = -kPi / 2.0f;
        roll = kPi;
        break;
    case POSITIVE_Z:
        yaw = kPi;
        roll = kPi;
        break;
    case NEGATIVE_X:
        yaw = kPi / 2.0f;
        roll = kPi;
        break;
    case POSITIVE_Y:
        pitch = kPi / 2.0f;
        break;
    case NEGATIVE_Y:
        pitch = -kPi / 2.0f;
        break;
    default:
        throw "Impossible case statment reached during face selection";
    }
    return Vector3(pitch, yaw, roll);
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

float VanDerCorputSequence(unsigned int index)
{
    index = (index << 16) | (index >> 16);
    index = ((index & 0x55555555) << 1) | ((index & 0xAAAAAAAA) >> 1);
    index = ((index & 0x33333333) << 2) | ((index & 0xCCCCCCCC) >> 2);
    index = ((index & 0x0F0F0F0F) << 4) | ((index & 0xF0F0F0F0) >> 4);
    index = ((index & 0x00FF00FF) << 8) | ((index & 0xFF00FF00) >> 8);
    // Inverse of 2^32, assuming 32-bit integer
    return static_cast<float>(index) * 2.3283064365386962890625e-10f;
}

Vector3 HsvToRgb(const Vector3& hsv)
{
    float hue = hsv.r * 6.0f;
    float sat = hsv.g;
    float val = hsv.b;

    int sector = std::min(static_cast<int>(hue), 5);
    float frac = std::fmodf(hue, 1.0f);

    float p = val * (1.0f - sat);
    float q = val * (1.0f - sat * frac);
    float t = val * (1.0f - sat * (1.0f - frac));

    switch (sector)
    {
    case 0:
        return Vector3(val, t, p);
    case 1:
        return Vector3(q, val, p);
    case 2:
        return Vector3(p, val, t);
    case 3:
        return Vector3(p, q, val);
    case 4:
        return Vector3(t, p, val);
    case 5:
        return Vector3(val, p, q);
    default:
        throw "Impossible case statement reached";
    }
}
} // namespace blons