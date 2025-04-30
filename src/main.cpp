#include <corutine.hpp>
#include <file.h>
#include <iostream>
int main()
{
    int fd = 0;
    SocketFile stdintest(fd);
    corutin* tasks[10] = {&stdintest};
    while (1)
    {
        for(auto& task:tasks)
        {
            task->eventGo();
        }
        std::cout << stdintest.read_added();
    }
}