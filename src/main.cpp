#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <iostream>
struct con
{
    int a = 0;
};

class mytask : public Task_Local<con>
{
    virtual Task<> co_fun() override
    {
        while (1)
        {
            a++;
            co_yield {};
        }
        co_return;
    }
};

int main()
{
    auto task = mytask{};
    std::cout << task.a << '\n';
    task.resume();

    std::cout << task.a << '\n';
    task.resume();

    std::cout << task.a << '\n';
    task.resume();

    std::cout << task.a << '\n';
    task.resume();

    return 0;
}