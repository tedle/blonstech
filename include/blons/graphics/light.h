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

#ifndef BLONSTECH_GRAPHICS_LIGHT_H_
#define BLONSTECH_GRAPHICS_LIGHT_H_

// Includes
#include <memory>
// Public Includes
#include <blons/math.h>

namespace blons
{
// Forward declarations
class Camera;

class Light
{
public:
    enum Type
    {
        DIRECTIONAL,
        POINT,
        SPOTLIGHT
    };

public:
    Light(Type type, Vector3 pos, Vector3 dir, Vector3 colour);
    Light(Type type)
        : Light(type, Vector3(), Vector3(0.0, -1.0, 0.0), Vector3(1.0, 1.0, 1.0)) {}
    Light()
        : Light(POINT) {}
    ~Light() {}

    // Used for shadow maps
    // Returns light's view*proj matrix
    Matrix ViewFrustum(Matrix frustum, units::world depth) const;

    const Vector3& colour() const;
    const Vector3& direction() const;
    const Vector3& pos() const;
    Matrix view_matrix() const;

    void set_colour(const Vector3& colour);
    void set_direction(const Vector3& dir);
    void set_pos(const Vector3& pos);

private:
    Type type_;
    Vector3 pos_;
    Vector3 direction_;
    Vector3 colour_;
    float intensity_;
    // Used for shadow maps
    std::unique_ptr<Camera> view_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Light
/// \ingroup graphics
///
/// ### Example:
/// \code
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_LIGHT_H_