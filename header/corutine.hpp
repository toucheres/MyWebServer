#pragma once
#include <any>
#include <coroutine>
#include <exception>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
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
        YIELD yield_result; // 存储yield的值
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
        bool is_yield_value = false;

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

template<class CONTEXT>
class Task_imp
{
  public:
    CONTEXT context;
    Task<void, void> corutine;

  public:
    // 构造函数和析构函数
    Task_imp() noexcept : corutine(nullptr)
    {
    }

    // 接受一个协程函数并创建内部协程
    template <typename Func> Task_imp(Func&& func) noexcept
    {
        corutine = func(std::ref(context));
    }

    // 新增：简化构造 - 直接接受协程函数
    template <typename Func, 
              typename = std::enable_if_t<std::is_invocable_r_v<Task<void, void>, Func, CONTEXT&>>>
    static Task_imp create(Func&& func) {
        Task_imp task;
        task.corutine = func(task.context);
        return task;
    }

    Task_imp(Task_imp&& other) noexcept
        : context(std::move(other.context)), corutine(std::move(other.corutine))
    {
    }

    Task_imp& operator=(Task_imp&& other) noexcept
    {
        if (this != &other)
        {
            context = std::move(other.context);
            corutine = std::move(other.corutine);
        }
        return *this;
    }

    ~Task_imp() = default;

    // 禁止拷贝
    Task_imp(const Task_imp&) = delete;
    Task_imp& operator=(const Task_imp&) = delete;

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
    CONTEXT& get_context() noexcept
    {
        return context;
    }

    const CONTEXT& get_context() const noexcept
    {
        return context;
    }
};

// 全局辅助函数，简化 Task_imp 的创建
template <typename CONTEXT, typename Func>
auto make_task(Func&& func) {
    return Task_imp<CONTEXT>::create(std::forward<Func>(func));
}

// 函数特征类，用于提取函数参数类型
template<typename T>
struct function_traits;

// 特化版本：普通函数
template<typename R, typename... Args>
struct function_traits<R(Args...)> {
    using return_type = R;
    static constexpr std::size_t arity = sizeof...(Args);
    
    template <std::size_t N>
    struct argument {
        static_assert(N < arity, "索引越界");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
    
    using first_argument_type = typename argument<0>::type;
};

// 特化版本：函数引用
template<typename R, typename... Args>
struct function_traits<R(&)(Args...)> : function_traits<R(Args...)> {};

// 特化版本：函数指针
template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {};

// 特化版本：成员函数
template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(Args...)> {};

// 特化版本：const成员函数
template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const> : function_traits<R(Args...)> {};

// 特化版本：callable对象
template<typename F>
struct function_traits {
    using call_type = function_traits<decltype(&F::operator())>;
    using return_type = typename call_type::return_type;
    static constexpr std::size_t arity = call_type::arity - 1; // 减去this指针
    
    template <std::size_t N>
    struct argument {
        static_assert(N < arity, "索引越界");
        using type = typename call_type::template argument<N+1>::type;
    };
    
    using first_argument_type = typename argument<0>::type;
};

// 修复版 make_task_auto 函数
template <typename Func>
auto make_task_auto(Func&& func) {
    // 使用函数特征类提取第一个参数类型
    using ContextRefType = typename function_traits<std::remove_reference_t<Func>>::first_argument_type;
    using ContextType = std::remove_reference_t<std::remove_const_t<ContextRefType>>;
    
    return Task_imp<ContextType>::create(std::forward<Func>(func));
}