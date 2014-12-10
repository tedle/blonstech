#ifndef BLONSTECH_MATH_UNITS_H_
#define BLONSTECH_MATH_UNITS_H_

namespace blons
{
namespace units
{
typedef int pixel;
typedef float subpixel;
typedef float world;

// Conversion functions
inline subpixel pixel_to_subpixel(pixel p)
{
    return static_cast<subpixel>(p);
}

inline pixel subpixel_to_pixel(subpixel p)
{
    return static_cast<pixel>(p);
}

namespace time
{
typedef unsigned __int64 ms;
typedef unsigned __int64 us;

// Conversion functions
inline ms us_to_ms(us t)
{
    return t / 1000;
}

inline us ms_to_us(ms t)
{
    return t * 1000;
}
} // namespace time
} // namespace units
} // namespace blons
#endif