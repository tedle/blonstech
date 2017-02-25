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

// TODO: Separate classes for different light types
// Sun should be measured in illuminance (lux)
// Punctual lights in lumens
// IBLs in luminance (candela/m^2)
////////////////////////////////////////////////////////////////////////////////
/// \brief Represents lights in a scene
////////////////////////////////////////////////////////////////////////////////
class Light
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Determines the behaviour of the light
    ////////////////////////////////////////////////////////////////////////////////
    enum Type
    {
        DIRECTIONAL, ///< Light rays are parallele and come from an infinite distance
        POINT,       ///< Light rays are sent in all directions from a point in space
        SPOTLIGHT    ///< Light rays are sent in a cone from a point in space
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new light using the supplied values
    ///
    /// \param type Type of light to create
    /// \param pos Position of the light. Irrelevant for directional lights
    /// \param dir Direction the light is pointed. Irrelevant for point lights
    /// \param colour Colour of light emitted
    /// \param luminance Intensity of the light in candela/meters^2
    ////////////////////////////////////////////////////////////////////////////////
    Light(Type type, Vector3 pos, Vector3 dir, Vector3 colour, units::luminance luminance);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls Light(Type, Vector3, Vector3, Vector3) with a default direction
    /// of straight down and a colour of white
    ////////////////////////////////////////////////////////////////////////////////
    Light(Type type)
        : Light(type, Vector3(), Vector3(0.0, -1.0, 0.0), Vector3(1.0, 1.0, 1.0), 1.0) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls Light(Type, Vector3, Vector3, Vector3) with a default type of
    /// Type::POINT and a colour of white
    ////////////////////////////////////////////////////////////////////////////////
    Light()
        : Light(POINT) {}
    ~Light() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Generates a view projection matrix that tightly surrounds the given
    /// view frustum from the light's point of view. Generally used for shadow maps
    ///
    /// \param frustum View projection matrix of the camera rendering the scene
    /// \param depth Render distance in world units
    ////////////////////////////////////////////////////////////////////////////////
    Matrix ViewFrustum(Matrix frustum, units::world depth) const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the light's emitted colour
    ///
    /// \return Vector3 containing colour information
    ////////////////////////////////////////////////////////////////////////////////
    const Vector3& colour() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the light's emitted direction
    ///
    /// \return Vector3 containing direction
    ////////////////////////////////////////////////////////////////////////////////
    const Vector3& direction() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the light's emitted intensity in candela/meters^2
    ///
    /// \return Luminance in candela/meters^2
    ////////////////////////////////////////////////////////////////////////////////
    const units::luminance& luminance() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the light's position in space
    ///
    /// \return Vector3 containing position
    ////////////////////////////////////////////////////////////////////////////////
    const Vector3& pos() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the light's view matrix based on position and direction
    ///
    /// \return View matrix
    ////////////////////////////////////////////////////////////////////////////////
    Matrix view_matrix() const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the light's emitted colour
    ///
    /// \param colour Vector3 containing colour information
    ////////////////////////////////////////////////////////////////////////////////
    void set_colour(const Vector3& colour);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the light's emitted direction
    ///
    /// \param dir Vector3 containing direction
    ////////////////////////////////////////////////////////////////////////////////
    void set_direction(const Vector3& dir);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the light's emitted luminance
    ///
    /// \param luminance Luminance in candela/meters^2
    ////////////////////////////////////////////////////////////////////////////////
    void set_luminance(const units::luminance& luminance);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Sets the light's position in space
    ///
    /// \param pos Vector3 containing position
    ////////////////////////////////////////////////////////////////////////////////
    void set_pos(const Vector3& pos);

private:
    Type type_;
    Vector3 pos_;
    Vector3 direction_;
    Vector3 colour_;
    float luminance_;
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