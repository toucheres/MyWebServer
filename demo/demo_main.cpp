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
    auto str = json{person{}};
    std::cout << json{json::parse<person>(str)};
    return 0;
}