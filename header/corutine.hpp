#pragma once
#include <chrono>
#include <coroutine>
#include <cstddef>
#include <exception>
#include <memory>
#include <thread>
#include <variant>
#include <vector>

enum class EventStatus
{
    Continue,
    Stop
};

class co_async
{
  public:
    virtual EventStatus eventGo() = 0;
    virtual ~co_async() = default;
};

// 泛型协程任务，可以返回任意类型T的值
template <typename RET = void, typename YIELD = void> class Task
{
  public:
    // 协程承诺类型
    class promise_type
    {
      private:
        RET result;
        YIELD yield_result; // 存储yield的值
        std::exception_ptr exception = nullptr;
        std::coroutine_handle<> continuation = nullptr;

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
            result = std::move(value);
        }

        void return_value(const RET& value) noexcept
        {
            result = value;
        }

        std::suspend_always yield_value(YIELD value) noexcept
        {
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

        friend class Task<RET, YIELD>;
    };

    // 特殊的协程等待器，用于在一个协程中等待另一个协程完成
    struct Awaiter
    {
      private:
        std::coroutine_handle<promise_type> handle;

      public:
        Awaiter(std::coroutine_handle<promise_type> h) : handle(h)
        {
        }

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
        std::coroutine_handle<> continuation = nullptr;

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

        friend class Task<void, YIELD>;
    };

    // void类型的等待器
    struct Awaiter
    {
      private:
        std::coroutine_handle<promise_type> handle;

      public:
        Awaiter(std::coroutine_handle<promise_type> h) : handle(h)
        {
        }

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
    operator bool()
    {
        return this->handle != nullptr;
    }
    // 协程承诺类型
    class promise_type
    {
      private:
        std::exception_ptr exception = nullptr;
        bool is_yield_value = false;
        std::coroutine_handle<> continuation = nullptr;

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

        // 添加对空 yield 的支持
        std::suspend_always yield_value(std::monostate) noexcept
        {
            is_yield_value = true;
            return {};
        }

        void get_result()
        {
            if (exception)
            {
                std::rethrow_exception(exception);
            }
        }

        bool is_yield() const noexcept
        {
            return is_yield_value;
        }

        friend class Task<void, void>;
    };

    // void类型的等待器
    struct Awaiter
    {
      private:
        std::coroutine_handle<promise_type> handle;

      public:
        Awaiter(std::coroutine_handle<promise_type> h) : handle(h)
        {
        }

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
    Task(std::nullptr_t)
    {
        Task();
    }
    Task(std::coroutine_handle<promise_type> h) noexcept : handle(h)
    {
    }

    Task(Task&& other) noexcept : handle(other.handle)
    {
        // this->handle = other.handle;
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
    Task& operator=(std::nullptr_t) noexcept
    {
        if (handle)
        {
            handle.destroy();
        }
        handle = nullptr;
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

    // 添加检查是否是 yield 的方法
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
template <class CONTEXT = void> class Task_Local : public CONTEXT
{
  public:
    virtual Task<> co_fun() = 0;

  private:
    Task<void, void> task_handle = nullptr;

  public:
    // 默认构造函数
    Task_Local() noexcept = default;

    // 移动构造
    Task_Local(Task_Local&& other) = delete;

    // 移动赋值
    Task_Local& operator=(Task_Local&& other) = delete;

    // nullptr赋值
    Task_Local& operator=(std::nullptr_t) noexcept
    {
        task_handle = nullptr;
        return *this;
    }

    // bool操作符
    operator bool()
    {
        return (bool)task_handle;
    }

    // 禁止拷贝
    Task_Local(const Task_Local&) = delete;
    Task_Local& operator=(const Task_Local&) = delete;

    // 协程操作接口
    bool done() const noexcept
    {
        return (!(bool)task_handle) || task_handle.done();
    }

    void resume()
    {
        if (!task_handle)
        {
            task_handle = co_fun();
        }
        task_handle.resume();
    }

    void get_result()
    {
        if (task_handle)
        {
            task_handle.get_result();
        }
    }

    // 检查是否是yield状态
    bool is_yield() const noexcept
    {
        if (!(bool)task_handle)
            return false;
        return task_handle.is_yield();
    }
};
template <class CONTEXT> class Task_Away
{
  private:
    std::shared_ptr<CONTEXT> context;
    Task<void, void> corutine;

  public:
    // 构造函数和析构函数
    Task_Away() noexcept : corutine(nullptr)
    {
    }

    // 接受一个协程函数并创建内部协程
    template <typename Func> Task_Away(Func&& func) noexcept
    {
        context = std::make_shared<CONTEXT>();
        corutine = func(context);
    }

    template <typename Func> Task_Away(std::shared_ptr<CONTEXT> p) noexcept
    {
        context = p;
        corutine = func(context);
    }

    // 直接接受协程函数指针，无需lambda包装
    template <typename R, typename Y> Task_Away(Task<R, Y> (*func)(CONTEXT&)) noexcept
    {
        context = std::make_shared<CONTEXT>();
        corutine = func(context);
    }

    Task_Away(Task_Away&& other) noexcept
        : context(other.context), corutine(std::move(other.corutine))
    {
        if (this != &other)
        {
            other.corutine = nullptr;
            other.context = nullptr;
        }
    }

    Task_Away& operator=(Task_Away&& other) noexcept
    {
        if (this != &other)
        {
            context = std::move(other.context);
            corutine = std::move(other.corutine);
            other.corutine = nullptr;
            other.context = nullptr;
        }
        return *this;
    }
    Task_Away& operator=(std::nullptr_t) noexcept
    {
        context.reset();
        context = nullptr;
        this->corutine = nullptr;
        return *this;
    }
    operator bool()
    {
        return context != nullptr;
    }

    ~Task_Away() = default;

    // 禁止拷贝
    Task_Away(const Task_Away&) = delete;
    Task_Away& operator=(const Task_Away&) = delete;

    // 协程操作接口 - 转发到内部协程
    bool done() const noexcept
    {
        return corutine.done();
    }

    void resume() const
    {
        corutine.resume();
    }

    void get_result()
    {
        corutine.get_result();
    }

    // 获取上下文
    std::shared_ptr<CONTEXT> get_context() noexcept
    {
        return context;
    }

    const std::shared_ptr<CONTEXT> get_context() const noexcept
    {
        return context;
    }
};

class Co_Manager
{
  private:
    std::vector<co_async*> tasks;

  public:
    void go()
    {
        std::vector<co_async*> tasksCopy = tasks;
        for (auto& task : tasksCopy)
        {
            if (task != nullptr)
            {
                if (task->eventGo() == EventStatus::Stop)
                {
                    remove(task);
                }
            }
        }
    }
    bool add(co_async* newtask)
    {
        tasks.emplace_back(newtask);
        return true;
    }
    bool add(co_async& newtask)
    {
        tasks.emplace_back(&newtask);
        return true;
    }
    bool remove(co_async* uselesstask)
    {
        for (auto& task : tasks)
        {
            if (task == uselesstask)
            {
                task = nullptr;
            }
        }
        return true;
    }
};

class Co_Start_Manager
{
  private:
    std::chrono::nanoseconds loopTime{1000000}; // 默认1毫秒
    Co_Manager manager;
    Co_Start_Manager() = default;
    Co_Start_Manager(Co_Start_Manager&&) = delete;
    Co_Start_Manager(const Co_Start_Manager&) = delete;
    Co_Start_Manager& operator=(Co_Start_Manager&&) = delete;
    Co_Start_Manager& operator=(const Co_Start_Manager&) = delete;

  public:
    // 设置循环间隔
    template <typename Rep, typename Period>
    void setLoopInterval(const std::chrono::duration<Rep, Period>& interval)
    {
        loopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(interval);
    }

    int start()
    {
        auto now = std::chrono::system_clock::now();
        auto end = std::chrono::system_clock::now();

        while (true)
        {
            now = std::chrono::system_clock::now();
            manager.go();
            end = std::chrono::system_clock::now();
            std::this_thread::sleep_for(loopTime - (end - now));
        }
    }
    static Co_Start_Manager& getInstance()
    {
        static auto instance = Co_Start_Manager{};
        return instance;
    }
    Co_Manager& getManager()
    {
        return manager;
    }
    bool add(co_async* newtask)
    {
        return manager.add(newtask);
    }
    bool add(co_async& newtask)
    {
        return manager.add(newtask);
    }
    bool remove(co_async* uselesstask)
    {
        return manager.remove(uselesstask);
    }
};