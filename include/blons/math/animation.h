////////////////////////////////////////////////////////////////////////////////
/// blonstech
/// Copyright(c) 2014 Dominic Bowden
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files(the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef BLONSTECH_MATH_ANIMATION_H_
#define BLONSTECH_MATH_ANIMATION_H_

// Includes
#include <functional>
// Public Includes
#include <blons/system/timer.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Utility class for general animation and easing effects
////////////////////////////////////////////////////////////////////////////////
class Animation
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Available easing functions for animations
    ////////////////////////////////////////////////////////////////////////////////
    enum TweenType
    {
        LINEAR,       ///< 1-to-1 linear function
        QUAD_IN,      ///< Quadratic (x^2) function
        QUAD_OUT,     ///< Inverse quadratic (x^2) function
        QUAD_IN_OUT,  ///< Mixed quadratic (x^2) function
        CUBIC_IN,     ///< Cubic (x^3) function
        CUBIC_OUT,    ///< Inverse cubic (x^3) function
        CUBIC_IN_OUT, ///< Mixed cubic (x^3) function
        QUINT_IN,     ///< Quintic (x^5) function
        QUINT_OUT,    ///< Inverse quintic (x^5) function
        QUINT_IN_OUT, ///< Mixed quintic (x^5) function
        SMOOTHSTEP    ///< Generic smoothing function
    };
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Function prototype for animation callbacks, receives a number from
    /// [0.0, 1.0] indicating the completion progress of the animation
    ////////////////////////////////////////////////////////////////////////////////
    typedef std::function<void(float)> Callback;

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new animation
    ///
    /// \param duration Total running time of the animation in milliseconds
    /// \param callback Function for the animation to act on whenever
    /// Animation::Update is called. Receives a floating point value from 0.0 to 1.0
    /// \param tween_method Easing algorithm to use for the animation
    ////////////////////////////////////////////////////////////////////////////////
    Animation(units::time::ms duration, Callback callback, TweenType tween_method);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls Animation(units::time::ms, Callback, TweenType) with a default
    /// easing function of TweenType::LINEAR
    ////////////////////////////////////////////////////////////////////////////////
    Animation(units::time::ms duration, Callback callback)
        : Animation(duration, callback, LINEAR) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates an empty animation with no callback
    ////////////////////////////////////////////////////////////////////////////////
    Animation()
        : Animation(0, [](float){}) {}
    ~Animation() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Restarts animation from the beginning
    ////////////////////////////////////////////////////////////////////////////////
    void Reset();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Completely halts animation
    ////////////////////////////////////////////////////////////////////////////////
    void Stop();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief To be called during an update cycle, performs easing calculations
    /// and invokes the callback with a value ranging from 0.0 to 1.0 indicating the
    /// progress of the animation. If the animation has already been completed in an
    /// earlier call to Update then the callback is not invoked
    ///
    /// \return Returns true on completion of the entire animation
    ////////////////////////////////////////////////////////////////////////////////
    bool Update();

private:
    bool completed_;

    Callback callback_;
    units::time::ms duration_;
    Timer timer_;
    TweenType tween_method_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Animation
/// \ingroup math
///
/// Animations are performed through a user defined callback that is supplied
/// during initialization and is invoked whenever Animation::Update is called.
///
/// ### Example:
/// \code
/// // Create a new animation with a 1 second duration
/// // that prints its progress to the debug log
/// blons::Animation::Callback cb = [](float d)
/// {
///     blons::log::Debug("%.1f%% complete!\n", d * 100);
/// };
/// blons::Animation animation(1000, cb, blons::Animation::SMOOTHSTEP);
///
/// // Main update loop, containing application code and stuff
/// while (true)
/// {
///     if (animation.Update())
///     {
///         blons::log::Debug("Animation completed!\n");
///         break;
///     }
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_MATH_ANIMATION_H_