#pragma once
#include "reflection.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

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
    template <typename Callable, 
             typename = std::enable_if_t<!std::is_same_v<std::decay_t<Callable>, std::function<void(Args...)>>>>
    void connect(Callable&& callable);

    void connect(enable_auto_remove_slots& owner, std::function<void(Args...)>&& callback);
    
    // 支持任意可调用对象的connect，带owner，自动适配参数数量
    template <typename Callable,
             typename = std::enable_if_t<!std::is_same_v<std::decay_t<Callable>, std::function<void(Args...)>>>>
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
        call_with_first_n_args(callable, args_tuple);
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
        call_with_first_n_args(callable, args_tuple);
    };
    slots.emplace_back(adapter, owner_ptr);
    owner.on_exit.connect([this, owner_ptr]() { this->remove(owner_ptr); });
}