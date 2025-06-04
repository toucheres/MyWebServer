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
    constexpr int member_count = num_of_number_v<person>;
    std::cout << "Member count: " << member_count << std::endl;

    if constexpr (member_count > 0)
    {
        constexpr auto members = make_static_memberptr_tuple_form_type<person>();
        std::cout << "1 member address: " << std::get<0>(members) << std::endl;
        std::cout << "2 member address: " << std::get<1>(members) << std::endl;
        std::cout << "3 member address: " << std::get<2>(members) << std::endl;
    }
    std::cout << bias_member<person, 0>()<<'\n';
    std::cout << bias_member<person, 1>()<<'\n';
    std::cout << bias_member<person, 2>()<<'\n';
    return 0;
}