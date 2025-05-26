#include "demo_util.h"
#include <chrono>
#include <sstream>
#include <iomanip>

std::string generateRandomSessionId(size_t length)
{
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(length);

    // 使用随机设备作为种子
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

    for (size_t i = 0; i < length; ++i)
    {
        result += alphanum[dis(gen)];
    }

    return result;
}

std::string formatTime(std::time_t time)
{
    char buffer[30];
    std::tm* timeinfo = std::localtime(&time);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}