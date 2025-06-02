#include "http.h"
#include "mysqlHandle.h"
#include "reflection.hpp"
MySQLHandle mysqldb;
HttpServer httpServer;
int main()
{
    person{10, "test"};
    // person{AnyType{}, AnyType{}};

    num_of_number_v<person>;
    is_same_all_v<AnyType, AnyType, AnyType>;

    auto lam = [](auto&&... arg) { (std::cout << ... << arg) << '\n'; };
    auto per = person{12, "testname"};
    visit_members(per, lam);
    num_of_number<person>::value;
    is_same_all<AnyType, AnyType, AnyType>::value;
    static int b = 0;
    auto a = getname<&b>{};
    // gcc 13.3.0:
    // void getname<p>::test() [with int* p = (& b)]
    // clang 18.1.3:
    // void getname<&b>::test() [p = &b]
    a.test();
}