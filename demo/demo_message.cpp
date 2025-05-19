#include "demo_message.h"

Message::Message(std::string asformat)
{
    // 解析格式化字符串，例如 "sender:张三\ncontent:你好\ntime:2023-06-01 12:30:45"
    size_t pos = 0;
    size_t end = 0;

    // 解析发送者
    if ((pos = asformat.find("sender:")) != std::string::npos)
    {
        pos += 7; // "sender:"的长度
        end = asformat.find('\n', pos);
        if (end != std::string::npos)
        {
            sender = asformat.substr(pos, end - pos);
        }
    }

    // 解析内容
    if ((pos = asformat.find("content:")) != std::string::npos)
    {
        pos += 8; // "content:"的长度
        end = asformat.find('\n', pos);
        if (end != std::string::npos)
        {
            content = asformat.substr(pos, end - pos);
        }
    }

    // 解析时间
    if ((pos = asformat.find("time:")) != std::string::npos)
    {
        pos += 5; // "time:"的长度
        end = asformat.find('\n', pos);
        if (end != std::string::npos)
        {
            time = asformat.substr(pos, end - pos);
        }
        else
        {
            time = asformat.substr(pos); // 如果没有换行符就取到末尾
        }
    }
}

Message::Message(std::vector<std::string> asformat)
{
    // 根据数据库实际结构调整索引
    // 数据库返回的行按照id, sender, content, timestamp, created_at的顺序
    if (asformat.size() >= 5)
    {
        sender = asformat[1];  // 发送者在索引1
        content = asformat[2]; // 内容在索引2
        // 使用created_at作为时间
        time = asformat[4]; // created_at在索引4
    }
}

Message::operator std::string()
{
    return "sender: " + sender + '\n' + "content: " + content + '\n' + "time: " + time + '\n' +
           '\n';
}
