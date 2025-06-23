#include "threading/thread_pool.hpp"
#include <thread>

namespace mtfs::threading {

void ThreadPool::start() {
    std::unique_lock<std::mutex> lock(poolMutex);
    if (running) return;
    running = true;
    paused = false;
    size_t threadCount = std::thread::hardware_concurrency();
    for (size_t i = 0; i < threadCount; ++i) {
        workers.emplace_back([this] { workerFunction(); });
    }
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(poolMutex);
        running = false;
        paused = false;
    }
    condition.notify_all();
    for (auto& worker : workers) {
        if (worker.joinable()) worker.join();
    }
    workers.clear();
}

void ThreadPool::pause() {
    std::unique_lock<std::mutex> lock(poolMutex);
    paused = true;
}

void ThreadPool::resume() {
    {
        std::unique_lock<std::mutex> lock(poolMutex);
        paused = false;
    }
    condition.notify_all();
}

bool ThreadPool::isRunning() const {
    std::unique_lock<std::mutex> lock(poolMutex);
    return running;
}

bool ThreadPool::isPaused() const {
    std::unique_lock<std::mutex> lock(poolMutex);
    return paused;
}

size_t ThreadPool::activeThreads() const {
    std::unique_lock<std::mutex> lock(poolMutex);
    return activeWorkers;
}

size_t ThreadPool::queuedTasks() const {
    std::unique_lock<std::mutex> lock(poolMutex);
    return tasks.size();
}

void ThreadPool::workerFunction() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(poolMutex);
            condition.wait(lock, [this] { return !running || (!paused && !tasks.empty()); });
            if (!running && tasks.empty()) return;
            if (paused) continue;
            if (!tasks.empty()) {
                task = std::move(tasks.front());
                tasks.pop();
                ++activeWorkers;
            }
        }
        if (task) {
            try { task(); } catch (...) {}
            {
                std::unique_lock<std::mutex> lock(poolMutex);
                if (activeWorkers > 0) --activeWorkers;
            }
        }
    }
}

ThreadPool::ThreadPool() {}
ThreadPool::~ThreadPool() { stop(); }

} // namespace mtfs::threading