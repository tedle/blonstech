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
    microseconds_ -= units::time::ms_to_us(ms);
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