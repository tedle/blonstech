#include "os/timer.h"

// Includes
#include <Windows.h>

namespace blons
{
Timer::Timer()
{
    milliseconds_ = 0;
    time_offset_ = 0;
    paused_ = true;
}

void Timer::start()
{
    paused_ = false;
    time_offset_ = GetTickCount64();
}

void Timer::pause()
{
    paused_ = true;
    milliseconds_ += GetTickCount64() - time_offset_;
}

void Timer::stop()
{
    paused_ = true;
    milliseconds_ = 0;
    time_offset_ = 0;
}

time_t Timer::ms()
{
    milliseconds_ += GetTickCount64() - time_offset_;
    time_offset_ = GetTickCount64();
    return milliseconds_;
}
} // namespace blons