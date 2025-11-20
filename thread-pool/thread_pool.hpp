#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <cstddef>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

/** 
 * @class ThreadPool
 * @brief manages a fixed number of worker threads that execute queued tasks
*/
class ThreadPool{
public:

    /** 
     * @brief starts specified number of worker threads
     * @param n - number of worker threads that handle tasks
    */
    explicit ThreadPool(size_t n);

    /** 
     * @brief stopping all worker threads
    */
    ~ThreadPool();

    /** 
     * @brief adding new task to the queue
     * @param f - task that is being added to queue
     * @throws std::runtime_error - when called after all worker threads stopped working
    */
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
    /// vector of threads owned by thread pool
    std::vector<std::thread> threads;
    /// queue of tasks
    std::queue<std::function<void()>> tasks;
    /// mutex for concurrent access to tasks
    std::mutex mtx;
    /// condition variable for waiting/notifying
    std::condition_variable cv;
    /// flag that handles stoppage of worker threads
    bool stop;
};

#endif
