#include <corutine.hpp>
#include <iostream>
#include <variant>
Task_Context<void,void> fun()
LOCAL
{
    int a = 0;
}
FUNC
{
    context.a = 10;
    co_yield;
    context.a = 12;
    co_return;
}
int main()
{
    auto handle = fun();
    std::cout << handle.context.a;
    handle.resume();
    std::cout << handle.context.a;
    handle.resume();
    std::cout << handle.context.a;
}