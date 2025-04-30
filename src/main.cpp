#include <corutine.hpp>
#include <iostream>
#include <variant>
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
    // 创建 Task_imp 并直接传入协程函数，无需lambda包装
    Task_imp task = test_coroutine;

    std::cout << task.context.data << "\n";
    task.resume();
    std::cout << task.context.data << "\n";

    // 打印上下文中的数据
    // std::cout << "Context data: " << task.get_context().data << std::endl;
}