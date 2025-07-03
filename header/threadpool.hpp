#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class threadpool
{
  private:
    std::vector<std::jthread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mtx;
    std::condition_variable cv;
    std::atomic<bool> stop{false};

  public:
    threadpool(size_t size) : threads(size)
    {
        for (auto& each : threads)
        {
            each = std::jthread{
                [this](std::stop_token stoken)
                {
                    while (!stoken.stop_requested() && !stop)
                    {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(tasks_mtx);
                            cv.wait(lock, [this, &stoken]
                                    { return !tasks.empty() || stoken.stop_requested() || stop; });

                            if (stoken.stop_requested() || stop)
                                break;

                            if (!tasks.empty())
                            {
                                task = std::move(tasks.front());
                                tasks.pop();
                            }
                        }

                        if (task)
                        {
                            task();
                        }
                    }
                }};
        }
    }

    template <typename F> void enqueue(F&& f)
    {
        {
            std::lock_guard<std::mutex> lock(tasks_mtx);
            tasks.emplace(std::forward<F>(f));
        }
        cv.notify_one();
    }

    ~threadpool()
    {
        stop = true;
        cv.notify_all();
        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.request_stop();
            }
        }
    }
};