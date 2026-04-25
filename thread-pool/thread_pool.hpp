#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <cstddef>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <concepts>

/**
 * @namespace util::concurrency
 * @brief module for utilities related to concurrency
*/
namespace util::concurrency {
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

        /// deleted copy constructor
        ThreadPool(const ThreadPool&) = delete;

        /// deleted assignment operator
        ThreadPool& operator=(const ThreadPool&) = delete;

        /// deleted move constructor
        ThreadPool(ThreadPool&&) noexcept = delete;

        /// deleted move assignment operator
        ThreadPool& operator=(ThreadPool&&) noexcept = delete;

        /** 
         * @brief adding new task to the queue
         * @tparam Fn - invocable type
         * @param fn - task that is being added to queue
         * @throws std::runtime_error - when called after all worker threads stopped working
        */
        template<std::invocable Fn>
        void enqueue(Fn&& fn){
            {
                std::lock_guard<std::mutex> lock(mtx);
                if(stopping){
                    throw std::runtime_error("Enqueue on stopped thread pool\n");
                }
                tasks.emplace(std::forward<Fn>(fn));
            }

            cv.notify_one();
        }

    private:
        /// mutex for concurrent access to tasks
        std::mutex mtx;

        /// condition variable for waiting/notifying
        std::condition_variable cv;
        
        /// flag that handles stoppage of worker threads
        bool stopping{false};

        /// queue of tasks
        std::queue<std::move_only_function<void()>> tasks;

        /// worker threads
        std::vector<std::thread> workers;

    };

}

#endif