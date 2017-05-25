////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef BLONSTECH_SYSTEM_JOB_H_
#define BLONSTECH_SYSTEM_JOB_H_

// Includes
#include <atomic>
#include <functional>

namespace blons
{
// Forward declarations
namespace internal { class ThreadPool; }

////////////////////////////////////////////////////////////////////////////////
/// \brief Asynchronous task to be run by internal worker threads
////////////////////////////////////////////////////////////////////////////////
class Job
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Function prototype for a Job to run
    ////////////////////////////////////////////////////////////////////////////////
    using Function = std::function<void()>;

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new Job that will execute the given function asynchronously
    /// after it has been queued
    ///
    /// \param func Function to be executed
    ////////////////////////////////////////////////////////////////////////////////
    Job(Function func);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Automatically calls Wait() on destruction to prevent data races and
    /// ensure job completion
    ////////////////////////////////////////////////////////////////////////////////
    ~Job();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Adds self to a global job queue to be consumed by worker threads
    ////////////////////////////////////////////////////////////////////////////////
    void Enqueue();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Blocks until all invocations of this job have been completed
    ////////////////////////////////////////////////////////////////////////////////
    void Wait();

private:
    // Actual function can only be started by worker threads
    friend class internal::ThreadPool;
    void Run();

    Function func_;
    std::atomic<int> running_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Job
/// \ingroup system
///
/// ### Example:
/// \code
/// // Create a new job
/// blons::Job job([]() { blons::log::Debug("Working!"); });
/// blons::log::Debug("Starting job... ");
/// // Insert it into the job queue and let it run asynchronously
/// job.Enqueue();
/// // When you need the results, call the blocking Wait() function
/// job.Wait();
/// // And we're done!
/// blons::log::Debug("Job complete!");
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_SYSTEM_JOB_H_