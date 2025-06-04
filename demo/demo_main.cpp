#include "reflection.hpp"
#include <iostream>
#include <string>

struct person
{
    int a;
    std::string b;
    int c;
};

int main()
{
    visit_members(person{1, "2", 3}, [](auto... args) { std::cout << (... << args); });
    return 0;
}