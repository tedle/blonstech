////////////////////////////////////////////////////////////////////////////////
/// blonstech
/// Copyright(c) 2014 Dominic Bowden
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files(the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/system/timer.h>

namespace blons
{
namespace // Stop gap platform isolation
{
// Quarantine this sucker
#include <Windows.h>

blons::units::time::us GetMicroseconds()
{
    static LARGE_INTEGER ticks, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&ticks);
    return (ticks.QuadPart * 1000 * 1000) / frequency.QuadPart;
}
} // namespace

Timer::Timer()
{
    stop();
    start();
}

void Timer::start()
{
    // Restart if not paused
    if (!paused_)
    {
        microseconds_ = 0;
    }
    paused_ = false;
    time_offset_ = GetMicroseconds();
}

void Timer::pause()
{
    paused_ = true;
    microseconds_ += GetMicroseconds() - time_offset_;
}

void Timer::stop()
{
    paused_ = true;
    microseconds_ = 0;
    time_offset_ = 0;
}

void Timer::rewind(units::time::ms ms)
{
    // Overflow check
    if (units::time::ms_to_us(ms) > microseconds_)
    {
        microseconds_ -= units::time::ms_to_us(ms);
    }
    else
    {
        microseconds_ = 0;
    }
}

units::time::ms Timer::ms()
{
    return units::time::us_to_ms(us());
}

units::time::us Timer::us()
{
    if (!paused_)
    {
        microseconds_ += GetMicroseconds() - time_offset_;
        time_offset_ = GetMicroseconds();
    }
    return microseconds_;
}
} // namespace blons