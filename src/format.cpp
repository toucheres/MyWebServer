#include <cstdio>
#include <cstring>
#include <format.h>
#include <regex>
#include <vector>

Format::Format(std::string format, Type type) : format_str(std::move(format)), type_(type)
{
}

bool Format::match(const std::string& str) const
{
    switch (type_)
    {
    case Type::same:
        return format_str == str;
    case Type::prefix:
        return str.compare(0, format_str.size(), format_str) == 0;
    case Type::suffix:
    {
        if (str.size() < format_str.size())
            return false;
        return str.compare(str.size() - format_str.size(), format_str.size(), format_str) == 0;
    }
    case Type::regex:
    {
        try
        {
            std::regex pattern(format_str);
            return std::regex_match(str, pattern);
        }
        catch (const std::regex_error&)
        {
            return false;
        }
    }
    case Type::scanf:
    {
        // 分析格式字符串，找出所有格式说明符
        int formatCount = 0;
        for (size_t i = 0; i < format_str.size(); ++i)
        {
            if (format_str[i] == '%' && i + 1 < format_str.size() && format_str[i + 1] != '%')
            {
                formatCount++;
            }
        }

        // 准备临时变量用于scanf匹配
        std::vector<void*> buffers;
        for (int i = 0; i < formatCount; ++i)
        {
            buffers.push_back(new char[1024]); // 使用足够大的缓冲区
            memset(buffers.back(), 0, 1024);
        }

        // 执行scanf
        int matches = 0;
        if (!buffers.empty())
        {
            matches = sscanf(str.c_str(), format_str.c_str(), buffers[0],
                             buffers.size() > 1 ? buffers[1] : nullptr,
                             buffers.size() > 2 ? buffers[2] : nullptr,
                             buffers.size() > 3 ? buffers[3] : nullptr,
                             buffers.size() > 4 ? buffers[4] : nullptr,
                             buffers.size() > 5 ? buffers[5] : nullptr,
                             buffers.size() > 6 ? buffers[6] : nullptr,
                             buffers.size() > 7 ? buffers[7] : nullptr,
                             buffers.size() > 8 ? buffers[8] : nullptr,
                             buffers.size() > 9 ? buffers[9] : nullptr);
        }

        // 清理内存
        for (auto ptr : buffers)
        {
            delete[] static_cast<char*>(ptr);
        }

        // 如果有成功匹配的数量等于格式说明符的数量，则认为匹配成功
        return matches > 0;
    }
    default:
        return false;
    }
}

bool Format::match(std::string_view str) const
{
    return match(std::string(str));
}

bool Format::operator==(const std::string& str) const
{
    return match(str);
}

bool Format::operator==(const Format& in) const
{
    // 两个Format对象只有在类型相同且格式字符串相同时才相等
    return type_ == in.type_ && format_str == in.format_str;
}

bool Format::operator<(const Format& in) const
{
    // 先比较类型，再比较格式字符串
    if (type_ != in.type_)
    {
        return static_cast<int>(type_) < static_cast<int>(in.type_);
    }
    return format_str < in.format_str;
}

std::optional<Format::ParseResult> Format::parse(const std::string& str) const
{
    switch (type_)
    {
    case Type::regex:
        return parseRegex(str);
    case Type::scanf:
        return parseScanf(str);
    default:
        // 其他类型不支持解析，只进行匹配
        if (match(str))
        {
            return ParseResult{}; // 返回空结果表示匹配成功但没有捕获
        }
        return std::nullopt;
    }
}

std::optional<Format::ParseResult> Format::parseScanf(const std::string& str) const
{
    // 首先分析格式字符串，找出所有的格式说明符
    std::vector<char> formatTypes;
    for (size_t i = 0; i < format_str.size(); ++i)
    {
        if (format_str[i] == '%')
        {
            if (i + 1 < format_str.size() && format_str[i + 1] != '%')
            {
                // 找到格式说明符的类型字符
                size_t j = i + 1;
                
                // 处理扫描集 %[...]
                if (j < format_str.size() && format_str[j] == '[')
                {
                    // 对于扫描集，我们将其视为 's' 类型（字符串）
                    formatTypes.push_back('s');
                    continue;
                }
                
                // 处理普通格式说明符
                while (j < format_str.size() && !std::isalpha(format_str[j]))
                {
                    ++j;
                }
                if (j < format_str.size())
                {
                    formatTypes.push_back(format_str[j]);
                }
            }
        }
    }

    // 准备用于scanf的缓冲区
    std::vector<void*> buffers;
    ParseResult result;

    for (char type : formatTypes)
    {
        switch (type)
        {
        case 'd':
        case 'i':
        {
            buffers.push_back(new int());
            break;
        }
        case 'f':
        case 'g':
        case 'e':
        {
            buffers.push_back(new double());
            break;
        }
        case 's':
        {
            const size_t bufSize = 1024;
            buffers.push_back(new char[bufSize]);
            memset(buffers.back(), 0, bufSize);
            break;
        }
            // 其他类型可以根据需要添加
        }
    }

    // 执行scanf
    int matches = 0;
    if (!buffers.empty())
    {
        std::vector<void*> scanfArgs = buffers;
        matches = sscanf(str.c_str(), format_str.c_str(), scanfArgs[0],
                         scanfArgs.size() > 1 ? scanfArgs[1] : nullptr,
                         scanfArgs.size() > 2 ? scanfArgs[2] : nullptr,
                         scanfArgs.size() > 3 ? scanfArgs[3] : nullptr,
                         scanfArgs.size() > 4 ? scanfArgs[4] : nullptr,
                         scanfArgs.size() > 5 ? scanfArgs[5] : nullptr,
                         scanfArgs.size() > 6 ? scanfArgs[6] : nullptr,
                         scanfArgs.size() > 7 ? scanfArgs[7] : nullptr,
                         scanfArgs.size() > 8 ? scanfArgs[8] : nullptr,
                         scanfArgs.size() > 9 ? scanfArgs[9] : nullptr);
    }

    // 如果匹配成功，处理结果
    if (matches > 0)
    {
        for (size_t i = 0; i < formatTypes.size() && i < buffers.size(); ++i)
        {
            switch (formatTypes[i])
            {
            case 'd':
            case 'i':
            {
                result.emplace_back(*static_cast<int*>(buffers[i]));
                delete static_cast<int*>(buffers[i]);
                break;
            }
            case 'f':
            case 'g':
            case 'e':
            {
                result.emplace_back(*static_cast<double*>(buffers[i]));
                delete static_cast<double*>(buffers[i]);
                break;
            }
            case 's':
            {
                result.emplace_back(std::string(static_cast<char*>(buffers[i])));
                delete[] static_cast<char*>(buffers[i]);
                break;
            }
                // 其他类型可以根据需要添加
            }
        }
        return result;
    }

    // 清理资源
    for (size_t i = 0; i < formatTypes.size() && i < buffers.size(); ++i)
    {
        switch (formatTypes[i])
        {
        case 'd':
        case 'i':
            delete static_cast<int*>(buffers[i]);
            break;
        case 'f':
        case 'g':
        case 'e':
            delete static_cast<double*>(buffers[i]);
            break;
        case 's':
            delete[] static_cast<char*>(buffers[i]);
            break;
            // 其他类型清理
        }
    }

    return std::nullopt;
}

std::optional<Format::ParseResult> Format::parseRegex(const std::string& str) const
{
    try
    {
        std::regex re(format_str);
        std::smatch matches;

        if (std::regex_search(str, matches, re))
        {
            ParseResult result;

            // 跳过整个匹配项（索引0），从子组开始
            for (size_t i = 1; i < matches.size(); ++i)
            {
                std::string match = matches[i].str();

                // 尝试将捕获组转换为数值类型
                try
                {
                    // 尝试转换为整数
                    size_t pos;
                    int intVal = std::stoi(match, &pos);
                    if (pos == match.size())
                    {
                        result.emplace_back(intVal);
                        continue;
                    }

                    // 尝试转换为浮点数
                    double doubleVal = std::stod(match, &pos);
                    if (pos == match.size())
                    {
                        result.emplace_back(doubleVal);
                        continue;
                    }
                }
                catch (const std::exception&)
                {
                    // 转换失败，将其视为字符串
                }

                // 默认为字符串
                result.emplace_back(match);
            }

            return result;
        }
    }
    catch (const std::regex_error&)
    {
        // 正则表达式错误
    }

    return std::nullopt;
}
