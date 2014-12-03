#include "os/timer.h"

// Includes
#include <Windows.h>

namespace blons
{
Timer::Timer()
{
    stop();
}

void Timer::start()
{
    // Restart if not paused
    if (!paused_)
    {
        microseconds_ = 0;
    }
    paused_ = false;
    time_offset_ = units::time::ms_to_us(GetTickCount64());
}

void Timer::pause()
{
    paused_ = true;
    microseconds_ += units::time::ms_to_us(GetTickCount64()) - time_offset_;
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
        microseconds_ += units::time::ms_to_us(GetTickCount64()) - time_offset_;
        time_offset_ = units::time::ms_to_us(GetTickCount64());
    }
    return microseconds_;
}
} // namespace blons