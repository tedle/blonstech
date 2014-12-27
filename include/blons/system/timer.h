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

#ifndef BLONSTECH_SYSTEM_TIMER_H_
#define BLONSTECH_SYSTEM_TIMER_H_

// Public Includes
#include <blons/math/units.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief High-resolution clock with stopping, restarting, etc
////////////////////////////////////////////////////////////////////////////////
class Timer
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes and starts counting a new timer
    ////////////////////////////////////////////////////////////////////////////////
    Timer();
    ~Timer() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Starts the timer. If the timer is already running or stopped, it is
    /// reset to zero before running. If it is paused the timer picks up where it
    /// left off
    ////////////////////////////////////////////////////////////////////////////////
    void start();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Pauses the timer and retains the current clock value
    ////////////////////////////////////////////////////////////////////////////////
    void pause();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Stops the timer and resets the clock value
    ////////////////////////////////////////////////////////////////////////////////
    void stop();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Rewinds the timer's clock value by the specified number of
    /// milliseconds
    ///
    /// \param ms Number of milliseconds to rewind by
    ////////////////////////////////////////////////////////////////////////////////
    void rewind(units::time::ms ms);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the amount of time elapsed since starting in milliseconds
    ///
    /// \return Number of milliseconds elapsed since start
    ////////////////////////////////////////////////////////////////////////////////
    units::time::ms ms();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the amount of time elapsed since starting in microseconds
    ///
    /// \return Number of microseconds elapsed since start
    ////////////////////////////////////////////////////////////////////////////////
    units::time::us us();

private:
    units::time::us microseconds_;
    units::time::us time_offset_;
    bool paused_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Timer
/// \ingroup system
///
/// ### Example:
/// \code
/// // Creating a new timer
/// blons::Timer timer;
///
/// // Performance test
/// timer.start();
/// SlowFunctionCall();
/// blons::log::Debug("Slow function took %ims!\n", timer.ms());
///
/// // Restart timer
/// timer.start();
///
/// // Endless loop that logs a message every second
/// while (true)
/// {
///     if (timer.ms() > 1000)
///     {
///         blons::log::Debug("Tick!\n");
///         timer.rewind(1000);
///     }
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_SYSTEM_TIMER_H_