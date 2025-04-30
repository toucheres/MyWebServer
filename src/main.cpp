#include <corutine.hpp>
#include <file.h>
#include <iostream>
int main()
{
    Co_Manager manager;
    int fd = 0;
    SocketFile stdintest(fd);
    manager.add(stdintest);
    while (1)
    {
        manager.go();
        std::cout << "get: " << stdintest.read_added();
        std::cout << "allget: " << stdintest.read_all();
    }
}