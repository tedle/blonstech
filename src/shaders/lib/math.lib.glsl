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

const float kPi = 3.1415926535897932384626433832795f;
const float kEpsilon = 0.000001f;

// These values should match the AxisAlignedNormal enum in includes/math/math.h
const int kPositiveX = 0;
const int kNegativeX = 1;
const int kPositiveY = 2;
const int kNegativeY = 3;
const int kPositiveZ = 4;
const int kNegativeZ = 5;

// These values must represent the AxisAlignedNormal enum in includes/math/math.h for each index
const vec3 kBasisDirections[6] = vec3[6](
    vec3( 1.0,  0.0,  0.0),
    vec3(-1.0,  0.0,  0.0),
    vec3( 0.0,  1.0,  0.0),
    vec3( 0.0, -1.0,  0.0),
    vec3( 0.0,  0.0,  1.0),
    vec3( 0.0,  0.0, -1.0)
);

vec3 SampleAmbientCube(const vec3 ambient_cube[6], const vec3 direction)
{
    vec3 direction_sq = direction * direction;
    ivec3 axis_direction = ivec3(direction.x > 0.0f ? kPositiveX : kNegativeX,
                                 direction.y > 0.0f ? kPositiveY : kNegativeY,
                                 direction.z > 0.0f ? kPositiveZ : kNegativeZ);
    vec3 colour = direction_sq.x * ambient_cube[axis_direction.x] +
                  direction_sq.y * ambient_cube[axis_direction.y] +
                  direction_sq.z * ambient_cube[axis_direction.z];
    return colour;
}

vec3 TriangleBarycentric(const vec3 tri[3], const vec3 plane_normal, const vec3 pos)
{
    // Implementation copied from C++ TriangleBarycentric
    vec3 barycentric;
    vec3 ray_origin = tri[0] - plane_normal;
    vec3 ray_dir = pos - ray_origin;
    vec3 edge1 = tri[1] - tri[0];
    vec3 edge2 = tri[2] - tri[0];

    vec3 P = cross(ray_dir, edge2);
    vec3 T = ray_origin - tri[0];
    vec3 Q = cross(T, edge1);
    float det = dot(edge1, P);
    if (abs(det) <= kEpsilon)
    {
        return vec3(0);
    }
    float inv_det = 1.0f / det;

    barycentric.y = dot(T, P) * inv_det;
    barycentric.z = dot(ray_dir, Q) * inv_det;
    barycentric.x = 1.0f - barycentric.y - barycentric.z;
    return barycentric;
}

float __sh_l0m0()
{
    // {l=0,m=0} = 1 / (2 * sqrt(pi))
    return 0.28209479177387814347403972578039f;
}

float __sh_l1m_1(const vec3 v)
{
    // {l=1,m=-1} = -(sqrt(3) * y) / (2 * sqrt(pi))
    return -0.48860251190291992158638462283835f * v.y;
}

float __sh_l1m0(const vec3 v)
{
    // {l=1,m=0} = (sqrt(3) * z) / (2 * sqrt(pi))
    return 0.48860251190291992158638462283835f * v.z;
}

float __sh_l1m1(const vec3 v)
{
    // {l=1,m=1} = -(sqrt(3) * x) / (2 * sqrt(pi))
    return -0.48860251190291992158638462283835f * v.x;
}

float __sh_l2m_2(const vec3 v)
{
    // {l=2,m=-2} = (sqrt(15) * y * x) / (2 * sqrt(pi))
    return 1.0925484305920790705433857058027f * v.y * v.x;
}

float __sh_l2m_1(const vec3 v)
{
    // {l=2,m=-1} = -(sqrt(15) * y * z) / (2 * sqrt(pi))
    return -1.0925484305920790705433857058027f * v.y * v.z;
}

float __sh_l2m0(const vec3 v)
{
    // {l=2,m=0} = (sqrt(5) * (3 * z^2 - 1)) / (4 * sqrt(pi))
    return 0.94617469575756001809268107088713f * v.z * v.z - 0.31539156525252000603089369029571f;
}

float __sh_l2m1(const vec3 v)
{
    // {l=2,m=1} = -(sqrt(15) * x * z) / (2 * sqrt(pi))
    return -1.0925484305920790705433857058027f * v.x * v.z;
}

float __sh_l2m2(const vec3 v)
{
    // {l=2,m=2} = (sqrt(15) * (x^2 - y^2)) / (4 * sqrt(pi))
    return 0.54627421529603953527169285290134f * (v.x * v.x - v.y * v.y);
}

float _kSHCosineLobe[3] = float[3](
    // pi
    3.1415926535897932384626433832795f,
    // 2 * pi / 3
    2.0943951023931954923084289221863f,
    // pi / 4
    0.78539816339744830961566084581988f
);

void SHProjectDirection2(vec3 direction, inout vec4 coefficients)
{
    coefficients = vec4(__sh_l0m0(), __sh_l1m_1(direction), __sh_l1m0(direction), __sh_l1m1(direction));
}

void SHProjectDirection3(vec3 direction, inout float coefficients[9])
{
    coefficients[0] = __sh_l0m0();
    coefficients[1] = __sh_l1m_1(direction);
    coefficients[2] = __sh_l1m0(direction);
    coefficients[3] = __sh_l1m1(direction);
    coefficients[4] = __sh_l2m_2(direction);
    coefficients[5] = __sh_l2m_1(direction);
    coefficients[6] = __sh_l2m0(direction);
    coefficients[7] = __sh_l2m1(direction);
    coefficients[8] = __sh_l2m2(direction);
}

void SHProjectCosineLobe2(vec3 direction, inout vec4 coefficients)
{
    coefficients = vec4(__sh_l0m0()           * _kSHCosineLobe[0],
                        __sh_l1m_1(direction) * _kSHCosineLobe[1],
                        __sh_l1m0(direction)  * _kSHCosineLobe[1],
                        __sh_l1m1(direction)  * _kSHCosineLobe[1]);
}

void SHProjectCosineLobe3(vec3 direction, inout float coefficients[9])
{
    coefficients[0] = __sh_l0m0()           * _kSHCosineLobe[0];
    coefficients[1] = __sh_l1m_1(direction) * _kSHCosineLobe[1];
    coefficients[2] = __sh_l1m0(direction)  * _kSHCosineLobe[1];
    coefficients[3] = __sh_l1m1(direction)  * _kSHCosineLobe[1];
    coefficients[4] = __sh_l2m_2(direction) * _kSHCosineLobe[2];
    coefficients[5] = __sh_l2m_1(direction) * _kSHCosineLobe[2];
    coefficients[6] = __sh_l2m0(direction)  * _kSHCosineLobe[2];
    coefficients[7] = __sh_l2m1(direction)  * _kSHCosineLobe[2];
    coefficients[8] = __sh_l2m2(direction)  * _kSHCosineLobe[2];
}

float SHDot2(vec4 coeffs_a, vec4 coeffs_b)
{
    return dot(coeffs_a, coeffs_b);
}

float SHDot3(float coeffs_a[9], float coeffs_b[9])
{
    float sh_dot = coeffs_a[0] * coeffs_b[0];
    sh_dot += coeffs_a[1] * coeffs_b[1];
    sh_dot += coeffs_a[2] * coeffs_b[2];
    sh_dot += coeffs_a[3] * coeffs_b[3];
    sh_dot += coeffs_a[4] * coeffs_b[4];
    sh_dot += coeffs_a[5] * coeffs_b[5];
    sh_dot += coeffs_a[6] * coeffs_b[6];
    sh_dot += coeffs_a[7] * coeffs_b[7];
    sh_dot += coeffs_a[8] * coeffs_b[8];
    return sh_dot;
}

// The Van der Corput sequence is a low discrepency model that works
// by reversing the bits of a number and shifting them behind a decimal
// Such that f(1011b) -> 0.1101b. We do an equivilent means
// of this by reversing the bits of an n-bit unsigned integer, casting
// the integer to a float, and then dividing by 2^n
// Efficient means of bit reversal explained more thoroughly:
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float VanDerCorputSequence(uint i)
{
    i = (i << 16) | (i >> 16);
    i = ((i & 0x55555555) << 1) | ((i & 0xAAAAAAAA) >> 1);
    i = ((i & 0x33333333) << 2) | ((i & 0xCCCCCCCC) >> 2);
    i = ((i & 0x0F0F0F0F) << 4) | ((i & 0xF0F0F0F0) >> 4);
    i = ((i & 0x00FF00FF) << 8) | ((i & 0xFF00FF00) >> 8);
    // Inverse of 2^32, assuming 32-bit integer
    return float(i) * 2.3283064365386962890625e-10f;
}

// Uses a Hammersley point set, which in 2D is simply a Van der Corput sequence paired
// with a linear progression of [0,1]
// See: http://mathworld.wolfram.com/HammersleyPointSet.html
vec2 LowDiscrepancySample2D(uint sample_id, uint sample_count)
{
    return vec2(float(sample_id) / float(sample_count), VanDerCorputSequence(sample_id));
}

// Generates a normal matrix from a basis normal vector
// For converting tangent-space coordinates to world-space
mat3 NormalVectorToMatrix(vec3 normal)
{
    vec3 up = abs(normal.z) > 0.9999f ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 0.0f, 1.0f);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);
    return mat3(tangent, bitangent, normal);
}

// Importance sampling function for GGX lobe, generates a likely significant
// halfway vector from a given random sample
// For derivation see:
// http://blog.tobias-franke.eu/2014/03/30/notes_on_importance_sampling.html
vec3 ImportanceSampleGGX(vec2 random_sample, vec3 normal, float roughness)
{
    float phi = random_sample.x * 2.0f * kPi;
    // We skip the calculation of theta since we only need the sine and cosine, anyway
    float theta_cos = sqrt((1.0f - random_sample.y) / ((roughness * roughness - 1.0f) * random_sample.y + 1.0f));
    // Pythageorean identity to get the sine of theta
    float theta_sin = sqrt(1.0f - theta_cos * theta_cos);
    // Get spherical normal from polar coordinates
    vec3 halfway = vec3(theta_sin * cos(phi), theta_sin * sin(phi), theta_cos);
    // Convert from tangent-space to world-space
    return NormalVectorToMatrix(normal) * halfway;
}

// Importance sampling function for cosine lobe, generates a likely significant
// light vector from a given random sample
vec3 ImportanceSampleCosineLobe(vec2 random_sample, vec3 normal)
{
    // Use the random sample to pick a point on the unit circle
    float phi = random_sample.y * kPi * 2.0f;
    // Radius of the slice of the hemisphere we are sampling (coplanar with the XY plane)
    float radius = sqrt(random_sample.x);
    vec3 light = vec3(radius * sin(phi), radius * cos(phi), sqrt(1.0f - random_sample.x));
    // Convert from tangent-space to world-space
    return NormalVectorToMatrix(normal) * light;
}