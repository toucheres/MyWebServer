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

std::string urlDecode(const std::string& encoded) 
{
    std::string result;
    result.reserve(encoded.length());

    for (size_t i = 0; i < encoded.length(); ++i) 
    {
        if (encoded[i] == '%' && i + 2 < encoded.length()) 
        {
            // 从十六进制字符转换为整数值
            int value;
            std::istringstream iss(encoded.substr(i + 1, 2));
            if (iss >> std::hex >> value) 
            {
                result += static_cast<char>(value);
                i += 2;  // 跳过已处理的两个十六进制字符
            } 
            else 
            {
                result += encoded[i];
            }
        } 
        else if (encoded[i] == '+') 
        {
            // 将加号转换为空格
            result += ' ';
        } 
        else 
        {
            result += encoded[i];
        }
    }

    return result;
}
