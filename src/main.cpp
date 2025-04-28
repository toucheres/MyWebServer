#include <corutine.hpp>
#include <http.h>
#include <iostream>
#include <string>

// 使用不同的yield和return类型
Task<int, std::string> mixed_types()
{
    for (size_t i = 0; i < 3; i++)
    {
        co_yield "This is yield #" + std::to_string(i);
    }
    co_return 42;
}
Task<int, int> test()
{
    for (size_t i = 0; i < 10; i++)
    {
        co_yield i;
    }
    co_return 11;
}
int main()
{
    // 测试相同类型
    auto t1 = test();
    while (!t1.done())
    {
        t1.resume();
        if (!t1.done())
        {
            std::cout << "Yielded: " << t1.get_yield_result() << std::endl;
        }
    }
    std::cout << "Final result: " << t1.get_result() << std::endl;

    // 测试不同类型
    auto t2 = mixed_types();
    while (!t2.done())
    {
        t2.resume();
        if (!t2.done())
        {
            if (t2.is_yield())
            {
                std::cout << "Yielded string: " << t2.get_yield_result() << std::endl;
            }
        }
    }
    std::cout << "Final int result: " << t2.get_result() << std::endl;

    return 0;
}