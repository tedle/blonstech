#ifndef BLONSTECH_OS_TIMER_H_
#define BLONSTECH_OS_TIMER_H_

// Includes
#include <cstddef>

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
    void rewind(time_t ms);

    time_t ms();

private:
    time_t milliseconds_;
    time_t time_offset_;
    bool paused_;
};
} // namespace blons
#endif