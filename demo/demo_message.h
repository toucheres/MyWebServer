#pragma once
#include <string>
#include <vector>

struct Message
{
    std::string sender = "";
    std::string content = "";
    std::string time = "1970-01-01 00:00::00";
    
    // 从格式化字符串创建消息 (sender:xxx\ncontent:xxx\ntime:xxx)
    Message(std::string asformat);
    
    // 从数据库查询结果创建消息
    Message(std::vector<std::string> asformat);
    
    // 转换为字符串格式
    operator std::string();
};
