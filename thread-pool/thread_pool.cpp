#include "thread_pool.hpp"
#include <mutex>
#include <condition_variable>

ThreadPool::ThreadPool(size_t numberOfThreads) : stop{ false } {
    for(size_t i = 0; i < numberOfThreads; ++i){
        threads.emplace_back([this](){
            while(true){
                std::function<void()> f;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this]{
                        return stop || !tasks.empty();
                    });

                    if(stop && tasks.empty()){
                        return;
                    }

                    f = std::move(tasks.front());
                    tasks.pop();
                }
                f();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();

    for(auto& t : threads){
        t.join();
    }
}
