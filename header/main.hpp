#pragma once
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <string>
class control : public co_async
{
    int eventGo()
    {
        std::string in{async_in_out.read_line()};
        if (in != "")
        {
            async_in_out.writeFile("get: ");
            async_in_out.writeFile(in);
            async_in_out.writeFile("\n");
        }
        return 0;
    }
};