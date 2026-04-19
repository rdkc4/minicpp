#include "thread_pool.hpp"

util::concurrency::ThreadPool::ThreadPool(size_t n){
    workers.reserve(n);

    for(size_t i{0}; i < n; ++i){
        workers.emplace_back([this]() -> void {
            while(true){
                std::move_only_function<void()> fn;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this] -> bool {
                        return stopping || !tasks.empty();
                    });

                    if(stopping && tasks.empty()){
                        return;
                    }

                    fn = std::move(tasks.front());
                    tasks.pop();
                }
                fn();
            }
        });
    }
}

util::concurrency::ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stopping = true;
    }
    cv.notify_all();

    for(auto& w : workers){
        w.join();
    }
}
