#include "math/animation.h"

namespace blons
{
Animation::Animation(units::time::ms duration, Callback callback, TweenType tween_method)
{
    completed_ = false;

    callback_ = callback;
    duration_ = duration;
    tween_method_ = tween_method;
    timer_.start();
}

bool Animation::Update()
{
    if (completed_)
    {
        return true;
    }

    auto elapsed = timer_.ms();
    if (elapsed > duration_)
    {
        completed_ = true;
        callback_(1.0f);
    }
    else
    {
        float completion = static_cast<float>(elapsed) / duration_;
        callback_(completion);
    }
    return completed_;
}
} // namespace blons