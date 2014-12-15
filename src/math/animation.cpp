#include <blons/math/animation.h>

namespace blons
{
namespace
{
// Interpolation functions
inline float linear(float x)
{
    return x;
}

inline float smoothstep(float x)
{
    return x*x*(3 - 2 * x);
}

inline float quad_in(float x)
{
    return x * x;
}

inline float quad_out(float x)
{
    return -1 * x * (x - 2);
}

inline float quad_in_out(float x)
{
    x *= 2;
    if (x < 1)
    {
        return 0.5f * x * x;
    }
    else
    {
        x--;
        return -0.5f * (x * (x - 2) - 1);
    }
}

inline float cubic_in(float x)
{
    return x * x * x;
}

inline float cubic_out(float x)
{
    x--;
    return (x * x * x + 1);
}

inline float cubic_in_out(float x)
{
    x *= 2;
    if (x < 1)
    {
        return 0.5f * x * x * x;
    }
    else
    {
        x -= 2;
        return 0.5f * (x * x * x + 2);
    }
}

inline float quint_in(float x)
{
    return x * x * x * x * x;
}

inline float quint_out(float x)
{
    x--;
    return (x * x * x * x * x + 1);
}

inline float quint_in_out(float x)
{
    x *= 2;
    if (x < 1)
    {
        return 0.5f * x * x * x * x * x;
    }
    else
    {
        x -= 2;
        return 0.5f * (x * x * x * x * x + 2);
    }
}
} // namespace

Animation::Animation(units::time::ms duration, Callback callback, TweenType tween_method)
{
    completed_ = false;

    callback_ = callback;
    duration_ = duration;
    tween_method_ = tween_method;
}

void Animation::Reset()
{
    completed_ = false;
    timer_.start();
}

void Animation::Stop()
{
    completed_ = true;
}

bool Animation::Update()
{
    if (completed_)
    {
        return true;
    }

    auto elapsed = timer_.us();
    if (elapsed > units::time::ms_to_us(duration_))
    {
        completed_ = true;
        callback_(1.0f);
    }
    else
    {
        float completion = static_cast<float>(elapsed) / units::time::ms_to_us(duration_);
        switch (tween_method_)
        {
        case LINEAR:
            completion = linear(completion);
            break;
        case QUAD_IN:
            completion = quad_in(completion);
            break;
        case QUAD_OUT:
            completion = quad_out(completion);
            break;
        case QUAD_IN_OUT:
            completion = quad_in_out(completion);
            break;
        case CUBIC_IN:
            completion = cubic_in(completion);
            break;
        case CUBIC_OUT:
            completion = cubic_out(completion);
            break;
        case CUBIC_IN_OUT:
            completion = cubic_in_out(completion);
            break;
        case QUINT_IN:
            completion = quint_in(completion);
            break;
        case QUINT_OUT:
            completion = quint_out(completion);
            break;
        case QUINT_IN_OUT:
            completion = quint_in_out(completion);
            break;
        case SMOOTHSTEP:
            completion = smoothstep(completion);
            break;
        default:
            throw "Unexpected interp function";
            break;
        }
        callback_(completion);
    }
    return completed_;
}
} // namespace blons