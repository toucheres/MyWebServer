#include "http.h"
#include "mysqlHandle.h"
#include "reflection.hpp"
MySQLHandle mysqldb;
HttpServer httpServer;
int main()
{
    std::cout << getname<person>::value[0]<<'\n';
    std::cout << getname<person>::value[1];
}