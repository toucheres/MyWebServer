#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <iostream>
int main()
{
    auto httpServer = HttpServer{};
    httpServer.start();
    return 0;
}