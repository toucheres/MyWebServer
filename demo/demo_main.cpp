#include "reflection.hpp"
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <ylt/reflection/member_names.hpp>
#include <ylt/reflection/member_ptr.hpp>


struct test
{
    bool a;
    std::string b;
};

int main()
{
    constexpr auto names = get_member_in_fun_names<test>();

    // 打印成员名称进行验证
    for (const auto& name : names)
    {
        std::cout << name << std::endl;
    }
    return 0;
}