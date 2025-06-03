#include "http.h"
#include "mysqlHandle.h"
#include "reflection.hpp"
#include <iostream>
#include <ylt/reflection/member_names.hpp>
MySQLHandle mysqldb;
HttpServer httpServer;
int main()
{
    auto lam = []<class mem, int index>(auto&& arg)
    {
        std::cout << "index: " << index << " " << struct_bias<mem, index>() << "content: " << arg
                  << std::endl;
    };
    visit_members_each_with_index(person{}, lam);
}