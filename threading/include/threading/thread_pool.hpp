#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include "common/error.hpp"

namespace mtfs::threading {

class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();

    // Disable copying
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Submit a task and get a future
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>;

    // Control methods
    void start();
    void stop();
    void pause();
    void resume();

    // Status methods
    bool isRunning() const;
    bool isPaused() const;
    size_t activeThreads() const;
    size_t queuedTasks() const;

private:
    void workerFunction();

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    mutable std::mutex poolMutex;
    std::condition_variable condition;
    
    bool running{false};
    bool paused{false};
    size_t activeWorkers{0};
};

// Implementation details will be in a separate .tpp file
#include "threading/thread_pool.tpp"

} // namespace mtfs::threading 