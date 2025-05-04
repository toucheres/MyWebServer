#include <chrono>
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <iostream>

int main()
{
    LocalFiles static_files;
    auto& coManager = Co_Start_Manager::getInstance();
    auto httpServer = HttpServer{};
    coManager.manager.add(httpServer);
    coManager.loopTime = std::chrono::nanoseconds(0);
    coManager.start();
    return 0;
}