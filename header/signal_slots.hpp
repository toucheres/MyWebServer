#pragma once
#include <functional>
#include <tuple>
#include <vector>
#include <type_traits>

// 辅助模板用于提取函数参数类型 - 添加默认参数
template <typename T, typename = void> 
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R(Args...), void> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
    static constexpr std::size_t args_count = sizeof...(Args);
    
    template <std::size_t N>
    using arg_type = std::tuple_element_t<N, args_tuple>;
};

template <typename R, typename... Args>
struct function_traits<R(*)(Args...), void> : function_traits<R(Args...)> {};

template <typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...), void> : function_traits<R(Args...)> {};

template <typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...) const, void> : function_traits<R(Args...)> {};

// 函数对象特化，使用SFINAE
template <typename T>
struct function_traits<T, std::void_t<decltype(&T::operator())>> 
    : function_traits<decltype(&T::operator())> {};

template <class... SignalArgs> 
class signal
{
    std::vector<std::function<void(SignalArgs...)>> slots;

public:
    void emit(SignalArgs... args)
    {
        for (auto&& callback : slots)
        {
            callback(std::forward<decltype(args)>(args)...);
        }
    }
    
    template <typename Callable>
    void connect(Callable&& callable)
    {
        using Traits = function_traits<std::decay_t<Callable>>;
        constexpr std::size_t callable_arg_count = Traits::args_count;
        
        if constexpr (callable_arg_count == 0) {
            // 无参数回调
            auto adapter = [callable = std::forward<Callable>(callable)](SignalArgs...) {
                callable();
            };
            slots.emplace_back(adapter);
        }
        else if constexpr (callable_arg_count <= sizeof...(SignalArgs)) {
            // 部分参数匹配，使用adapter从信号参数中提取前N个
            auto adapter = [callable = std::forward<Callable>(callable)]
                          (SignalArgs... args) {
                call_with_first_n_args(callable, std::forward<SignalArgs>(args)...);
            };
            slots.emplace_back(adapter);
        }
        else {
            // 无法连接
            static_assert(callable_arg_count <= sizeof...(SignalArgs), 
                          "Callable requires more arguments than signal provides");
        }
    }
    
private:
    // 调用辅助函数，提取前N个参数
    template <typename Callable, typename... CbArgs, typename... RemArgs>
    static void call_with_first_n_args(Callable&& cb, CbArgs&&... cb_args, RemArgs&&...) {
        if constexpr (sizeof...(CbArgs) == function_traits<std::decay_t<Callable>>::args_count) {
            cb(std::forward<CbArgs>(cb_args)...);
        } else {
            // 继续递归提取
            static_assert(sizeof...(CbArgs) < function_traits<std::decay_t<Callable>>::args_count,
                         "Too many arguments provided");
        }
    }
};