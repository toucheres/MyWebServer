#include "reflection.hpp"
#include <iostream>
#include <string>

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
    static constexpr int at = num_of_number_with_inner<person>::count;
    return 0;
}