#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <vector>
template <class... Args> class signal
{
    std::vector<std::function<void(Args...)>> slots;

  public:
    void emit(Args... args)
    {
        for (auto&& callback : slots)
        {
            callback(std::forward<decltype(args)>(args)...);
        }
    }
    void connect(std::function<void(Args...)>&& callback)
    {
        slots.emplace_back(callback);
    }
};