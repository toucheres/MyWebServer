#include <corutine.hpp>
#include <file.h>
#include <iostream>
int main()
{
    int fd = 0;
    SocketFile stdintest(fd);
    while (1)
    {
        stdintest.eventGo();
        std::cout << stdintest.read_added();
    }
}