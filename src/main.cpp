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
        std::cout << stdintest.read_added();
    }
}