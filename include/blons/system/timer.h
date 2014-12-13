#ifndef BLONSTECH_SYSTEM_TIMER_H_
#define BLONSTECH_SYSTEM_TIMER_H_

// Public Includes
#include <blons/math/units.h>

namespace blons
{
class Timer
{
public:
    Timer();
    ~Timer() {};

    void start();
    void pause();
    void stop();
    void rewind(units::time::ms ms);

    units::time::ms ms();
    units::time::us us();

private:
    units::time::us microseconds_;
    units::time::us time_offset_;
    bool paused_;
};
} // namespace blons

#endif // BLONSTECH_SYSTEM_TIMER_H_