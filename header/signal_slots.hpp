#pragma once
#include "reflection.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

// 函数特性提取工具
namespace detail
{
    // 获取函数的参数数量和类型
    template <typename T> struct function_traits;

    // 普通函数
    template <typename R, typename... Args> struct function_traits<R(Args...)>
    {
        static constexpr std::size_t arity = sizeof...(Args);
        using result_type = R;
        using args_tuple = std::tuple<Args...>;
    };

    // 函数指针
    template <typename R, typename... Args>
    struct function_traits<R (*)(Args...)> : function_traits<R(Args...)>
    {
    };

    // 成员函数指针
    template <typename C, typename R, typename... Args>
    struct function_traits<R (C::*)(Args...)> : function_traits<R(Args...)>
    {
    };

    // const 成员函数指针
    template <typename C, typename R, typename... Args>
    struct function_traits<R (C::*)(Args...) const> : function_traits<R(Args...)>
    {
    };

    // std::function
    template <typename R, typename... Args>
    struct function_traits<std::function<R(Args...)>> : function_traits<R(Args...)>
    {
    };

    // lambda和可调用对象
    template <typename F> struct function_traits
    {
      private:
        using call_type = function_traits<decltype(&F::operator())>;

      public:
        static constexpr std::size_t arity = call_type::arity;
        using result_type = typename call_type::result_type;
        using args_tuple = typename call_type::args_tuple;
    };

    // 移除引用和cv限定符
    template <typename F> struct function_traits<F&> : function_traits<F>
    {
    };

    template <typename F> struct function_traits<F&&> : function_traits<F>
    {
    };

    // 辅助函数：前N个参数调用
    template <typename F, typename Tuple, std::size_t... I>
    auto call_with_first_n_args_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
    {
        return f(std::get<I>(std::forward<Tuple>(t))...);
    }

    template <typename F, typename Tuple> auto call_with_first_n_args(F&& f, Tuple&& t)
    {
        constexpr std::size_t f_arity = function_traits<std::decay_t<F>>::arity;
        constexpr std::size_t tuple_size = std::tuple_size_v<std::decay_t<Tuple>>;
        constexpr std::size_t valid_args = (f_arity < tuple_size) ? f_arity : tuple_size;

        return call_with_first_n_args_impl(std::forward<F>(f), std::forward<Tuple>(t),
                                           std::make_index_sequence<valid_args>{});
    }
} // namespace detail

class enable_auto_remove_slots;
template <class... Args> class signal
{
    std::vector<std::pair<std::function<void(Args...)>, void*>> slots;

  public:
    void emit(Args... args)
    {
        for (auto&& callback : slots)
        {
            callback.first(std::forward<Args>(args)...); // 修复：调用 pair 的第一个元素
        }
    }

    void remove(void* owner)
    {
        auto it = std::remove_if(slots.begin(), slots.end(),
                                 [owner](const auto& in) { return in.second == owner; });
        if (it != slots.end())
        {
            slots.erase(it, slots.end());
        }
    }

    // 原始的接受完整参数的connect
    void connect(std::function<void(Args...)>&& callback)
    {
        slots.emplace_back(callback, nullptr);
    }

    // 支持任意可调用对象的connect，自动适配参数数量
    template <typename Callable, typename = std::enable_if_t<!std::is_same_v<
                                     std::decay_t<Callable>, std::function<void(Args...)>>>>
    void connect(Callable&& callable);

    void connect(enable_auto_remove_slots& owner, std::function<void(Args...)>&& callback);

    // 支持任意可调用对象的connect，带owner，自动适配参数数量
    template <typename Callable, typename = std::enable_if_t<!std::is_same_v<
                                     std::decay_t<Callable>, std::function<void(Args...)>>>>
    void connect(enable_auto_remove_slots& owner, Callable&& callable);
};

class enable_auto_remove_slots
{
  public:
    signal<> on_exit;

    virtual ~enable_auto_remove_slots()
    {
        on_exit.emit();
    }
};

// 将模板方法实现放在enable_auto_remove_slots类定义之后
template <class... Args>
template <typename Callable, typename>
void signal<Args...>::connect(Callable&& callable)
{
    auto adapter = [callable = std::forward<Callable>(callable)](Args... args)
    {
        auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
        detail::call_with_first_n_args(callable, args_tuple);
    };
    slots.emplace_back(adapter, nullptr);
}

template <class... Args>
void signal<Args...>::connect(enable_auto_remove_slots& owner,
                              std::function<void(Args...)>&& callback)
{
    void* owner_ptr = static_cast<void*>(&owner);
    slots.emplace_back(callback, owner_ptr);
    owner.on_exit.connect([this, owner_ptr]() { this->remove(owner_ptr); });
}

template <class... Args>
template <typename Callable, typename>
void signal<Args...>::connect(enable_auto_remove_slots& owner, Callable&& callable)
{
    void* owner_ptr = static_cast<void*>(&owner);
    auto adapter = [callable = std::forward<Callable>(callable)](Args... args)
    {
        auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
        detail::call_with_first_n_args(callable, args_tuple);
    };
    slots.emplace_back(adapter, owner_ptr);
    owner.on_exit.connect([this, owner_ptr]() { this->remove(owner_ptr); });
}