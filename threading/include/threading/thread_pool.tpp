#include "threading/thread_pool.hpp"

namespace mtfs::threading {

template<typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
    using ReturnType = typename std::invoke_result<F, Args...>::type;
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<ReturnType> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(poolMutex);
        if (!running) throw std::runtime_error("ThreadPool not running");
        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

} // namespace mtfs::threading
