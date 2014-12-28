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

#ifndef BLONSTECH_MATH_UNITS_H_
#define BLONSTECH_MATH_UNITS_H_

#include <stdint.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \ingroup math
/// \brief Various unit types for dealing with coordinates and time
////////////////////////////////////////////////////////////////////////////////
namespace units
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Measures pixel coordinates
////////////////////////////////////////////////////////////////////////////////
typedef int pixel;
////////////////////////////////////////////////////////////////////////////////
/// \brief Measures pixel coordinates with subpixel resolution
////////////////////////////////////////////////////////////////////////////////
typedef float subpixel;
////////////////////////////////////////////////////////////////////////////////
/// \brief Measures world space coordinates
////////////////////////////////////////////////////////////////////////////////
typedef float world;

////////////////////////////////////////////////////////////////////////////////
/// \brief Converts a given pixel coordinate to a subpixel coordinate
///
/// \param p Pixel coordinate to convert
/// \return Subpixel value
////////////////////////////////////////////////////////////////////////////////
inline subpixel pixel_to_subpixel(pixel p)
{
    return static_cast<subpixel>(p);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Converts a given subpixel coordinate to a pixel coordinate
///
/// \param p Subpixel coordinate to convert
/// \return Pixel value
////////////////////////////////////////////////////////////////////////////////
inline pixel subpixel_to_pixel(subpixel p)
{
    return static_cast<pixel>(p);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Unit types for dealing with time
////////////////////////////////////////////////////////////////////////////////
namespace time
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Measures time in millisecond resolution
////////////////////////////////////////////////////////////////////////////////
typedef uint64_t ms;
////////////////////////////////////////////////////////////////////////////////
/// \brief Measures time in microsecond resolution
////////////////////////////////////////////////////////////////////////////////
typedef uint64_t us;

////////////////////////////////////////////////////////////////////////////////
/// \brief Converts microsecond to milliseconds
///
/// \param t Microseconds to convert
/// \return Milliseconds
////////////////////////////////////////////////////////////////////////////////
inline ms us_to_ms(us t)
{
    return t / 1000;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Converts milliseconds to microseconds
///
/// \param t Milliseconds to convert
/// \return Microseconds
////////////////////////////////////////////////////////////////////////////////
inline us ms_to_us(ms t)
{
    return t * 1000;
}
} // namespace time
} // namespace units
} // namespace blons

#endif // BLONSTECH_MATH_UNITS_H_