#include "json.hpp"
#include <iostream>
#include <string>

struct person
{
    int a;
    std::string b;
};

int main()
{
    std::cout << json(person{1, "123"});
    return 0;
}