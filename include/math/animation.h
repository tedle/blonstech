#ifndef BLONSTECH_MATH_ANIMATION_H_
#define BLONSTECH_MATH_ANIMATION_H_

#include <functional>

namespace blons
{
class Animation
{
public:
    enum TweenType
    {
        LINEAR
    };
    typedef std::function<void(float)> Callback;
public:
    Animation(TweenType type, Callback callback, __int64 duration);
    ~Animation() {};

private:
    Timer timer_;
    TweenType type_;
};
} // namespace blons
#endif