#include "json.hpp"
#include "reflection.hpp"
#include <iostream>
#include <string>
#include <thread>

struct inner
{
    int aaa;
    int bbb;
};

struct person
{
    int a;
    std::string b;
    inner c;
};

int main()
{
    auto str = json::from(person{});
    std::cout << json::to<person>(str) << '\n';
    return 0;
}