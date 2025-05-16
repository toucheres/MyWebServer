#pragma once
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <string>
class control : public co_async
{
    EventStatus eventGo() // Changed return type
    {
        std::string in{async_in_out::getInstance().read_until()};
        if (in != "")
        {
            async_in_out::getInstance().writeFile("get: ");
            async_in_out::getInstance().writeFile(in);
            async_in_out::getInstance().writeFile("\n");
            if (in == "quit\n")
            {
                std::exit(0); // Exiting directly, so Stop might not be processed
            }
        }
        return EventStatus::Continue; // control task continues
    }
};