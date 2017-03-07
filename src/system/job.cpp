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

#include <blons/system/job.h>

// Includes
#include <array>
#include <atomic>
#include <queue>
// Public Includes
#include <blons/debug/log.h>
#include <blons/system/timer.h>

namespace blons
{
namespace internal
{
class ThreadedQueue
{
public:
    ThreadedQueue() {}
    ~ThreadedQueue() {}

    void push(Job* job)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        jobs_.push(job);
        // Unlock before notification because the transfer is
        // instant and the waiting variable would just instantly
        // relock otherwise
        lock.unlock();
        condition_variable_.notify_one();
    }

    // pop blocks until there is a Job to return or nullptr
    // if timeout is reached. A timeout of 0 blocks indefinitely
    Job* pop(units::time::ms timeout)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (jobs_.empty())
        {
            auto timed_out = condition_variable_.wait_for(lock, std::chrono::milliseconds(timeout));
            if (timed_out == std::cv_status::timeout)
            {
                return nullptr;
            }
        }
        auto job = jobs_.front();
        jobs_.pop();
        return job;
    }

private:
    std::queue<Job*> jobs_;
    std::mutex mutex_;
    std::condition_variable condition_variable_;
};

static ThreadedQueue g_JobQueue;

class ThreadPool
{
public:
    static const int kWorkerThreads = 3;

    ThreadPool()
    {
        // Set running state to true
        run_.store(true);
        // Launch all worker threads
        for (auto& worker : workers_)
        {
            worker = std::thread([&]()
            {
                // Query for jobs while ThreadPool is running
                while (run_.load())
                {
                    // Use a timeout so we can query run_ every once in a while
                    auto job = g_JobQueue.pop(300);
                    if (job != nullptr)
                    {
                        job->Run();
                    }
                }
            });
        }
    }
    ~ThreadPool()
    {
        run_.store(false);
        for (auto& worker : workers_)
        {
            worker.join();
        }
    }

private:
    std::atomic<bool> run_;
    std::array<std::thread, kWorkerThreads> workers_;
};

static ThreadPool g_ThreadPool;
} // namespace internal

Job::Job(Function func)
{
    func_ = func;
    async_queued_ = 0;
    running_ = 0;
}

Job::~Job()
{
    Wait();
}

void Job::Enqueue()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        async_queued_++;
    }
    internal::g_JobQueue.push(this);
}

void Job::Wait()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while(async_queued_ || running_)
    {
        condition_variable_.wait(lock);
    }
}

void Job::Run()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        async_queued_ = std::max(async_queued_ - 1, 0);
        running_++;
    }
    func_();
    {
        std::unique_lock<std::mutex> lock(mutex_);
        running_--;
        lock.unlock();
        condition_variable_.notify_one();
    }
}
} // namespace blons