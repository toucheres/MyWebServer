#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class threadpool
{
  public:
    enum class ShutdownMode {
        GRACEFUL,    // 优雅停止：等待所有任务完成
        IMMEDIATE,   // 立即停止：不接受新任务，等待当前任务完成
        FORCE        // 强制停止：立即终止所有线程
    };

  private:
    std::vector<std::jthread> threads;
    std::queue<std::function<void()>> tasks;
    mutable std::mutex tasks_mtx;
    std::condition_variable cv;
    std::atomic<bool> accept_new_tasks{true};  // 是否接受新任务
    std::atomic<bool> force_stop{false};       // 强制停止标志
    std::atomic<bool> paused{false};           // 暂停标志（新增）
    std::atomic<int> active_threads{0};        // 活跃线程计数
    std::condition_variable shutdown_cv;       // 用于等待线程完成
    std::condition_variable pause_cv;          // 用于暂停/恢复控制（新增）
    std::mutex shutdown_mtx;
    std::mutex pause_mtx;                      // 暂停控制互斥锁（新增）

  public:
    threadpool(size_t size) : threads(size)
    {
        for (auto& each : threads)
        {
            each = std::jthread{
                [this](std::stop_token stoken)
                {
                    while (!stoken.stop_requested() && !force_stop)
                    {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(tasks_mtx);
                            cv.wait(lock, [this, &stoken] {
                                return !tasks.empty() || 
                                       stoken.stop_requested() || 
                                       force_stop ||
                                       (!accept_new_tasks && tasks.empty());
                            });

                            // 检查退出条件
                            if (stoken.stop_requested() || force_stop) {
                                printf("Thread force stopped!\n");
                                break;
                            }
                            
                            // 优雅停止：处理完剩余任务后退出
                            if (!accept_new_tasks && tasks.empty()) {
                                printf("Thread gracefully stopped!\n");
                                break;
                            }

                            if (!tasks.empty()) {
                                task = std::move(tasks.front());
                                tasks.pop();
                            }
                        }

                        if (task) {
                            // 检查是否暂停（新增）
                            {
                                std::unique_lock<std::mutex> pause_lock(pause_mtx);
                                pause_cv.wait(pause_lock, [this, &stoken] {
                                    return !paused || stoken.stop_requested() || force_stop;
                                });
                                
                                // 如果在暂停期间收到停止信号，退出
                                if (stoken.stop_requested() || force_stop) {
                                    printf("Thread stopped during pause!\n");
                                    break;
                                }
                            }

                            ++active_threads;  // 开始执行任务
                            try {
                                task();
                            } catch (const std::exception& e) {
                                std::cerr << "Task execution error: " << e.what() << std::endl;
                            }
                            --active_threads;  // 任务完成
                            
                            // 通知可能在等待的shutdown操作
                            shutdown_cv.notify_all();
                        }
                    }
                }};
        }
    }

    template <typename F> 
    bool enqueue(F&& f)
    {
        if (!accept_new_tasks) {
            // std::cout << "Task rejected: threadpool not accepting new tasks!\n";
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(tasks_mtx);
            if (!accept_new_tasks) {  // 双重检查
                // std::cout << "Task rejected: threadpool not accepting new tasks!\n";
                return false;
            }
            // std::cout << "Task added!\n";
            tasks.emplace(std::forward<F>(f));
        }
        cv.notify_one();
        return true;
    }

    // 暂停任务执行（非阻塞）
    void stop()
    {
        if (paused) {
            // std::cout << "Threadpool is already paused.\n";
            return;
        }
        
        // std::cout << "Pausing threadpool...\n";
        paused = true;
        // std::cout << "Threadpool paused. Current running tasks will complete, but no new tasks will start.\n";
        wait_for_pause();
    }

    // 恢复任务执行（非阻塞）
    void start()
    {
        if (!paused) {
            // std::cout << "Threadpool is already running.\n";
            return;
        }
        
        // std::cout << "Resuming threadpool...\n";
        paused = false;
        pause_cv.notify_all();  // 唤醒所有等待的线程
        // std::cout << "Threadpool resumed.\n";
    }

    // 检查是否暂停
    bool is_paused() const {
        return paused;
    }

    // 等待所有活跃任务完成（用于确保暂停生效）
    void wait_for_pause()
    {
        if (!paused) {
            // std::cout << "Threadpool is not paused.\n";
            return;
        }
        
        // std::cout << "Waiting for all active tasks to complete...\n";
        std::unique_lock<std::mutex> lock(shutdown_mtx);
        shutdown_cv.wait(lock, [this] {
            return active_threads == 0;
        });
        // std::cout << "All active tasks completed. Threadpool is fully paused.\n";
    }

    // 停止接受新任务，等待现有任务完成（阻塞）
    void shutdown() 
    {
        // std::cout << "Threadpool shutdown initiated...\n";
        
        // 如果暂停中，先恢复以便处理剩余任务
        if (paused) {
            // std::cout << "Resuming paused threadpool for shutdown...\n";
            start();
        }
        
        // 停止接受新任务
        accept_new_tasks = false;
        cv.notify_all();  // 唤醒所有等待的线程

        // 等待所有任务完成
        std::unique_lock<std::mutex> lock(shutdown_mtx);
        shutdown_cv.wait(lock, [this] {
            std::lock_guard<std::mutex> tasks_lock(tasks_mtx);
            return tasks.empty() && active_threads == 0;
        });

        // std::cout << "All tasks completed. Threadpool shutdown complete.\n";
    }

    // 强制停止所有线程（非阻塞）
    void force_shutdown()
    {
        // std::cout << "Force shutdown initiated...\n";
        
        accept_new_tasks = false;
        force_stop = true;
        paused = false;  // 取消暂停状态
        cv.notify_all();
        pause_cv.notify_all();  // 唤醒暂停的线程

        // 请求所有线程停止
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.request_stop();
            }
        }

        // std::cout << "Force shutdown requested.\n";
    }

    // 获取线程池状态
    bool is_accepting_tasks() const {
        return accept_new_tasks;
    }

    size_t pending_tasks() const {
        std::lock_guard<std::mutex> lock(tasks_mtx);
        return tasks.size();
    }

    int active_task_count() const {
        return active_threads;
    }

    // 获取详细状态信息
    void print_status() const {
        std::cout << "=== Threadpool Status ===" << std::endl;
        std::cout << "Accepting tasks: " << (accept_new_tasks ? "Yes" : "No") << std::endl;
        std::cout << "Paused: " << (paused ? "Yes" : "No") << std::endl;
        std::cout << "Force stop: " << (force_stop ? "Yes" : "No") << std::endl;
        std::cout << "Pending tasks: " << pending_tasks() << std::endl;
        std::cout << "Active threads: " << active_task_count() << std::endl;
        std::cout << "=========================" << std::endl;
    }

    // 析构函数：默认优雅停止，等待所有任务完成（阻塞）
    ~threadpool()
    {
        if (accept_new_tasks) {
            // std::cout << "Threadpool destructor: initiating graceful shutdown...\n";
            shutdown();  // 优雅停止
        }

        // 等待所有线程结束
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        // std::cout << "Threadpool destructor completed.\n";
    }
};