#include "reflection.hpp"
#include <iostream>
#include <string>
#include <string_view>

struct test
{
    bool a;
    std::string b;
};
struct person
{
    int age;
    std::string name;
};
int main()
{
    constexpr auto names = get_member_names<test>();
    // 打印成员名称进行验证
    for (const auto& name : names)
    {
        std::cout << name << std::endl;
    }
    constexpr auto names_ = get_member_names<person>();
    // 打印成员名称进行验证
    for (const auto& name : names_)
    {
        std::cout << name << std::endl;
    }
    return 0;
}