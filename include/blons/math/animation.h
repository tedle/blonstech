#ifndef BLONSTECH_MATH_ANIMATION_H_
#define BLONSTECH_MATH_ANIMATION_H_

// Includes
#include <functional>
// Public Includes
#include <blons/os/timer.h>

namespace blons
{
class Animation
{
public:
    enum TweenType
    {
        LINEAR,
        QUAD_IN,
        QUAD_OUT,
        QUAD_IN_OUT,
        CUBIC_IN,
        CUBIC_OUT,
        CUBIC_IN_OUT,
        QUINT_IN,
        QUINT_OUT,
        QUINT_IN_OUT,
        SMOOTHSTEP
    };
    typedef std::function<void(float)> Callback;

public:
    // Callback is sent a progression value ranging from 0.0 to 1.0
    Animation(units::time::ms duration, Callback callback, TweenType tween_method);
    Animation(units::time::ms duration, Callback callback)
        : Animation(duration, callback, LINEAR) {}
    Animation()
        : Animation(0, [](float){}) {}
    ~Animation() {};

    // Restarts animation from beginning
    void Reset();
    // Completely halts animation
    void Stop();
    // Returns true on completion
    bool Update();

private:
    bool completed_;

    Callback callback_;
    units::time::ms duration_;
    Timer timer_;
    TweenType tween_method_;
};
} // namespace blons
#endif