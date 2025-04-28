#include <coroutine>
#include <exception>
#include <utility>
#include <variant>

// 泛型协程任务，可以返回任意类型T的值
template <typename RET = void, typename YIELD = void> class Task
{
  public:
    // 协程承诺类型
    class promise_type
    {
      private:
        RET result;
        YIELD yield_result;          // 存储yield的值
        bool is_yield_value = false; // 标记是否是yield的值
        std::exception_ptr exception = nullptr;

      public:
        Task get_return_object() noexcept
        {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        auto final_suspend() noexcept
        {
            class Awaiter
            {
              public:
                bool await_ready() noexcept
                {
                    return false;
                }
                void await_resume() noexcept
                {
                }
                std::coroutine_handle<> await_suspend(
                    std::coroutine_handle<promise_type> h) noexcept
                {
                    return h.promise().continuation ? h.promise().continuation
                                                    : std::noop_coroutine();
                }
            };

            return Awaiter{};
        }

        void unhandled_exception() noexcept
        {
            exception = std::current_exception();
        }

        void return_value(RET&& value) noexcept
        {
            is_yield_value = false;
            result = std::move(value);
        }

        void return_value(const RET& value) noexcept
        {
            is_yield_value = false;
            result = value;
        }

        std::suspend_always yield_value(YIELD value) noexcept
        {
            is_yield_value = true;
            yield_result = std::move(value);
            return {};
        }

        RET& get_result()
        {
            if (exception)
            {
                std::rethrow_exception(exception);
            }
            return result;
        }

        YIELD& get_yield_result()
        {
            if (exception)
            {
                std::rethrow_exception(exception);
            }
            return yield_result;
        }

        bool is_yield() const noexcept
        {
            return is_yield_value;
        }

        std::coroutine_handle<> continuation = nullptr;
    };

    // 特殊的协程等待器，用于在一个协程中等待另一个协程完成
    struct Awaiter
    {
        std::coroutine_handle<promise_type> handle;

        bool await_ready() const noexcept
        {
            return !handle || handle.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept
        {
            handle.promise().continuation = continuation;
            return handle;
        }

        RET await_resume()
        {
            return handle.promise().get_result();
        }
    };

    // 构造函数和析构函数
    Task() noexcept : handle(nullptr)
    {
    }

    Task(std::coroutine_handle<promise_type> h) noexcept : handle(h)
    {
    }

    Task(Task&& other) noexcept : handle(other.handle)
    {
        other.handle = nullptr;
    }

    Task& operator=(Task&& other) noexcept
    {
        if (this != &other)
        {
            if (handle)
            {
                handle.destroy();
            }
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    ~Task()
    {
        if (handle)
        {
            handle.destroy();
        }
    }

    // 禁止拷贝
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // 协程操作接口
    bool done() const noexcept
    {
        return !handle || handle.done();
    }

    void resume() const
    {
        if (handle)
        {
            handle.resume();
        }
    }

    // 获取yield值或return值
    RET get_result()
    {
        return handle.promise().get_result();
    }

    YIELD get_yield_result()
    {
        return handle.promise().get_yield_result();
    }

    // 判断当前值是否是通过yield返回的
    bool is_yield() const noexcept
    {
        if (!handle)
            return false;
        return handle.promise().is_yield();
    }

    // 转换为等待器，方便在其他协程中使用 co_await
    Awaiter operator co_await() const noexcept
    {
        return Awaiter{handle};
    }

  private:
    std::coroutine_handle<promise_type> handle;
};

// Task<void, YIELD> 的特化版本 - 支持void返回值但有yield值的情况
template <typename YIELD> class Task<void, YIELD>
{
  public:
    // 协程承诺类型
    class promise_type
    {
      private:
        YIELD yield_result;
        bool is_yield_value = false;
        std::exception_ptr exception = nullptr;

      public:
        Task get_return_object() noexcept
        {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        auto final_suspend() noexcept
        {
            class Awaiter
            {
              public:
                bool await_ready() noexcept
                {
                    return false;
                }
                void await_resume() noexcept
                {
                }
                std::coroutine_handle<> await_suspend(
                    std::coroutine_handle<promise_type> h) noexcept
                {
                    return h.promise().continuation ? h.promise().continuation
                                                    : std::noop_coroutine();
                }
            };

            return Awaiter{};
        }

        void unhandled_exception() noexcept
        {
            exception = std::current_exception();
        }

        void return_void() noexcept
        {
            is_yield_value = false;
        }

        std::suspend_always yield_value(YIELD value) noexcept
        {
            is_yield_value = true;
            yield_result = std::move(value);
            return {};
        }

        void get_result()
        {
            if (exception)
            {
                std::rethrow_exception(exception);
            }
        }

        YIELD& get_yield_result()
        {
            if (exception)
            {
                std::rethrow_exception(exception);
            }
            return yield_result;
        }

        bool is_yield() const noexcept
        {
            return is_yield_value;
        }

        std::coroutine_handle<> continuation = nullptr;
    };

    // void类型的等待器
    struct Awaiter
    {
        std::coroutine_handle<promise_type> handle;

        bool await_ready() const noexcept
        {
            return !handle || handle.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept
        {
            handle.promise().continuation = continuation;
            return handle;
        }

        void await_resume()
        {
            handle.promise().get_result();
        }
    };

    // 构造函数和析构函数
    Task() noexcept : handle(nullptr)
    {
    }

    Task(std::coroutine_handle<promise_type> h) noexcept : handle(h)
    {
    }

    Task(Task&& other) noexcept : handle(other.handle)
    {
        other.handle = nullptr;
    }

    Task& operator=(Task&& other) noexcept
    {
        if (this != &other)
        {
            if (handle)
            {
                handle.destroy();
            }
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    ~Task()
    {
        if (handle)
        {
            handle.destroy();
        }
    }

    // 禁止拷贝
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // 协程操作接口
    bool done() const noexcept
    {
        return !handle || handle.done();
    }

    void resume() const
    {
        if (handle)
        {
            handle.resume();
        }
    }

    void get_result()
    {
        handle.promise().get_result();
    }

    YIELD get_yield_result()
    {
        return handle.promise().get_yield_result();
    }

    bool is_yield() const noexcept
    {
        if (!handle)
            return false;
        return handle.promise().is_yield();
    }

    // 转换为等待器
    Awaiter operator co_await() const noexcept
    {
        return Awaiter{handle};
    }

  private:
    std::coroutine_handle<promise_type> handle;
};

// 保留原有的Task<void>特化版本
template <> class Task<void, void>
{
  public:
    // 协程承诺类型
    class promise_type
    {
      private:
        std::exception_ptr exception = nullptr;

      public:
        Task get_return_object() noexcept
        {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        auto final_suspend() noexcept
        {
            class Awaiter
            {
              public:
                bool await_ready() noexcept
                {
                    return false;
                }
                void await_resume() noexcept
                {
                }
                std::coroutine_handle<> await_suspend(
                    std::coroutine_handle<promise_type> h) noexcept
                {
                    return h.promise().continuation ? h.promise().continuation
                                                    : std::noop_coroutine();
                }
            };

            return Awaiter{};
        }

        void unhandled_exception() noexcept
        {
            exception = std::current_exception();
        }

        void return_void() noexcept
        {
        }

        void get_result()
        {
            if (exception)
            {
                std::rethrow_exception(exception);
            }
        }

        std::coroutine_handle<> continuation = nullptr;
    };

    // void类型的等待器
    struct Awaiter
    {
        std::coroutine_handle<promise_type> handle;

        bool await_ready() const noexcept
        {
            return !handle || handle.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept
        {
            handle.promise().continuation = continuation;
            return handle;
        }

        void await_resume()
        {
            handle.promise().get_result();
        }
    };

    // 构造函数和析构函数
    Task() noexcept : handle(nullptr)
    {
    }

    Task(std::coroutine_handle<promise_type> h) noexcept : handle(h)
    {
    }

    Task(Task&& other) noexcept : handle(other.handle)
    {
        other.handle = nullptr;
    }

    Task& operator=(Task&& other) noexcept
    {
        if (this != &other)
        {
            if (handle)
            {
                handle.destroy();
            }
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    ~Task()
    {
        if (handle)
        {
            handle.destroy();
        }
    }

    // 禁止拷贝
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // 协程操作接口
    bool done() const noexcept
    {
        return !handle || handle.done();
    }

    void resume() const
    {
        if (handle)
        {
            handle.resume();
        }
    }

    void get_result()
    {
        handle.promise().get_result();
    }

    // 转换为等待器
    Awaiter operator co_await() const noexcept
    {
        return Awaiter{handle};
    }

  private:
    std::coroutine_handle<promise_type> handle;
};
