#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace cg::platform {

// JobQueue 提供最小任务队列，负责后台 worker 线程的启动、停止与 join。
class JobQueue {
public:
    // workerCount 为后台 worker 数量；传入 0 时按 1 个 worker 创建。
    explicit JobQueue(std::size_t workerCount = 1);
    ~JobQueue();

    JobQueue(const JobQueue&) = delete;
    JobQueue& operator=(const JobQueue&) = delete;
    JobQueue(JobQueue&&) = delete;
    JobQueue& operator=(JobQueue&&) = delete;

    // Stop 停止接收新任务，等待已入队任务处理完成，并 join 所有 worker。
    void Stop();

    // Enqueue 添加任务；队列已停止时返回 false。
    [[nodiscard]] bool Enqueue(std::function<void()> job);

    // IsRunning 查询队列是否仍接收任务。
    [[nodiscard]] bool IsRunning() const;

private:
    void WorkerLoop();

    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<std::function<void()>> jobs_;
    std::vector<std::thread> workers_;
    bool running_ = true;
};

} // namespace cg::platform
