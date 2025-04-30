#include <corutine.hpp>
#include <iostream>
struct contype
{
    int data = 0;
};

// 返回一个协程任务
Task<void, void> test_coroutine(contype& context)
{
    context.data = 10;
    co_return;
}

int main()
{
    Task_Local task = test_coroutine;
    std::cout << task.context.data << "\n";
    task.resume();
    std::cout << task.context.data << "\n";
}