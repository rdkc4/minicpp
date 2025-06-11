#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <cstddef>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool{
public:
    explicit ThreadPool(size_t);
    ~ThreadPool();

    template<typename T>
    void enqueue(T&& f) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            if(stop){
                throw std::runtime_error("Enqueue on stopped thread\n");
            }
            tasks.emplace(std::forward<T>(f));
        }

        cv.notify_one();
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;
};

#endif
