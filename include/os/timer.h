#ifndef BLONSTECH_OS_TIMER_H_
#define BLONSTECH_OS_TIMER_H_

// Local Includes
#include "math/units.h"

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

private:
    units::time::ms milliseconds_;
    units::time::ms time_offset_;
    bool paused_;
};
} // namespace blons
#endif