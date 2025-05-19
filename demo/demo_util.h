#pragma once
#include <string>
#include <ctime>
#include <random>

// 生成随机会话ID
std::string generateRandomSessionId(size_t length = 32);

// 格式化时间
std::string formatTime(std::time_t time);
