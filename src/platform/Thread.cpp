#include "platform/Thread.hpp"

#include <utility>

namespace cg::platform {

JobQueue::JobQueue(std::size_t workerCount)
{
    const std::size_t actualWorkerCount = workerCount == 0 ? 1 : workerCount;
    workers_.reserve(actualWorkerCount);

    try {
        for (std::size_t i = 0; i < actualWorkerCount; ++i) {
            workers_.emplace_back(&JobQueue::WorkerLoop, this);
        }
    } catch (...) {
        Stop();
        throw;
    }
}

JobQueue::~JobQueue()
{
    Stop();
}

void JobQueue::Stop()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
    }

    condition_.notify_all();

    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

bool JobQueue::Enqueue(std::function<void()> job)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running_) {
            return false;
        }

        jobs_.push(std::move(job));
    }

    condition_.notify_one();
    return true;
}

bool JobQueue::IsRunning() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return running_;
}

void JobQueue::WorkerLoop()
{
    while (true) {
        std::function<void()> job;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]() {
                return !running_ || !jobs_.empty();
            });

            if (!running_ && jobs_.empty()) {
                return;
            }

            job = std::move(jobs_.front());
            jobs_.pop();
        }

        job();
    }
}

} // namespace cg::platform
