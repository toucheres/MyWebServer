#include "demo_message.h"
#include <sstream>

// 从格式化字符串创建消息
Message::Message(std::string asformat)
{
    std::istringstream iss(asformat);
    std::string line;

    while (std::getline(iss, line))
    {
        if (line.substr(0, 7) == "sender:")
        {
            sender = line.substr(7);
        }
        else if (line.substr(0, 8) == "content:")
        {
            content = line.substr(8);
        }
        else if (line.substr(0, 5) == "time:")
        {
            time = line.substr(5);
        }
    }
}

// 从数据库查询结果创建消息
Message::Message(std::vector<std::string> row)
{
    // 检查结果行是否有足够的列
    if (row.size() >= 3)
    {
        sender = row[1];    // sender列
        content = row[2];   // content列

        // 尝试获取创建时间
        if (row.size() >= 5)
        {
            time = row[4];  // created_at列
        }
    }
}

// 转换为字符串格式
Message::operator std::string()
{
    return "sender:" + sender + "\ncontent:" + content + "\ntime:" + time + "\n\n";
}
