#pragma once
#include "reflection.hpp"
#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

class json
{
  public:
    std::string content = "";
    template <Aggregate T> static json from(T in)
    {
        return json(in);
    }
    template <Aggregate T> static json to(json in)
    {
        return from_json<T>(in.content);
    }

  private:
    friend std::ostream& operator<<(std::ostream& os, const json& j)
    {
        return os << j.content;
    }
    template <Aggregate T> json(T in)
    {
        content += "{";
        visit_each_member(
            in,
            [this]<class M, int index>(auto&& arg)
            {
                using ArgType = std::decay_t<decltype(arg)>;

                // 添加类型检查和自定义错误信息
                static_assert(
                    requires { (std::string) arg; } || requires { std::to_string(arg); } ||
                        MeaningfulAggregate<ArgType>,
                    "不支持的成员类型：必须是可转为字符串、有 std::to_string 支持或嵌套聚合类");

                if constexpr (requires { (std::string) arg; })
                {
                    add_string(get_member_names<M>()[index], (std::string)arg);
                }
                else if constexpr (std::is_integral_v<ArgType> || std::is_floating_point_v<ArgType>)
                {
                    // 数值类型使用专门的数值处理方法
                    add_number(get_member_names<M>()[index], std::to_string(arg));
                }
                else if constexpr (requires { std::to_string(arg); })
                {
                    // 其他可以转为字符串但不是数值类型的情况
                    add_string(get_member_names<M>()[index], std::to_string(arg));
                }
                else if constexpr (MeaningfulAggregate<ArgType>)
                {
                    // 处理嵌套聚合类型
                    add_object(get_member_names<M>()[index], json(arg).operator std::string());
                }
                content += ',';
            });
        if (!content.empty() && content.back() == ',')
            content.pop_back();
        content += "}";
    }
    operator std::string()
    {
        return content;
    }

    // 重命名为add_string，更明确表示添加字符串值
    void add_string(std::string_view key, std::string_view value)
    {
        content += std::string_view("\"");
        content += key;
        content += "\":\"";
        content += value;
        content += "\"";
    }

    // 添加新方法专门处理数值类型（不使用引号）
    void add_number(std::string_view key, std::string_view value)
    {
        content += std::string_view("\"");
        content += key;
        content += "\":"; // 没有引号
        content += value; // 直接添加数值
    }

    // 保留原有方法，但标记为已过时
    [[deprecated("请使用更明确的add_string或add_number")]]
    void add(std::string_view left, std::string_view right)
    {
        add_string(left, right);
    }

    // 添加一个处理嵌套对象的方法
    void add_object(std::string_view key, std::string_view value)
    {
        content += std::string_view("\"");
        content += key;
        content += "\":";
        content += value;
    }

    // 添加 JSON 解析功能
    class parser
    {
      private:
        std::string_view json_str;
        size_t pos = 0;

        void skip_whitespace()
        {
            while (pos < json_str.size() && (json_str[pos] == ' ' || json_str[pos] == '\n' ||
                                             json_str[pos] == '\t' || json_str[pos] == '\r'))
                pos++;
        }

        std::string parse_string()
        {
            if (json_str[pos] != '"')
                throw std::runtime_error("Expected '\"' at position " + std::to_string(pos));

            pos++; // 跳过开始的双引号
            std::string result;
            while (pos < json_str.size() && json_str[pos] != '"')
            {
                if (json_str[pos] == '\\' && pos + 1 < json_str.size())
                {
                    pos++;
                    // 处理转义字符
                    switch (json_str[pos])
                    {
                    case '"':
                        result += '"';
                        break;
                    case '\\':
                        result += '\\';
                        break;
                    case '/':
                        result += '/';
                        break;
                    case 'b':
                        result += '\b';
                        break;
                    case 'f':
                        result += '\f';
                        break;
                    case 'n':
                        result += '\n';
                        break;
                    case 'r':
                        result += '\r';
                        break;
                    case 't':
                        result += '\t';
                        break;
                    default:
                        result += json_str[pos];
                    }
                }
                else
                {
                    result += json_str[pos];
                }
                pos++;
            }
            if (pos >= json_str.size())
                throw std::runtime_error("Unterminated string");

            pos++; // 跳过结束的双引号
            return result;
        }

        int parse_number()
        {
            size_t start = pos;
            while (pos < json_str.size() && (std::isdigit(json_str[pos]) || json_str[pos] == '-' ||
                                             json_str[pos] == '+' || json_str[pos] == '.'))
                pos++;

            std::string_view num_str = json_str.substr(start, pos - start);
            int result;
            auto [ptr, ec] =
                std::from_chars(num_str.data(), num_str.data() + num_str.size(), result);
            if (ec != std::errc())
                throw std::runtime_error("Failed to parse number at position " +
                                         std::to_string(start));

            return result;
        }

        std::unordered_map<std::string, std::string> parse_object()
        {
            if (json_str[pos] != '{')
                throw std::runtime_error("Expected '{' at position " + std::to_string(pos));

            std::unordered_map<std::string, std::string> result;
            pos++; // 跳过 '{'
            skip_whitespace();

            // 空对象情况
            if (pos < json_str.size() && json_str[pos] == '}')
            {
                pos++;
                return result;
            }

            while (pos < json_str.size())
            {
                skip_whitespace();
                // 解析键
                std::string key = parse_string();

                skip_whitespace();
                if (pos >= json_str.size() || json_str[pos] != ':')
                    throw std::runtime_error("Expected ':' after key at position " +
                                             std::to_string(pos));

                pos++; // 跳过 ':'
                skip_whitespace();

                // 解析值
                size_t value_start = pos;
                if (json_str[pos] == '"')
                {
                    // 字符串值
                    result[key] = parse_string();
                }
                else if (json_str[pos] == '{')
                {
                    // 嵌套对象值 - 存储为原始JSON字符串
                    int nested_level = 1;
                    pos++; // 跳过 '{'

                    while (nested_level > 0 && pos < json_str.size())
                    {
                        if (json_str[pos] == '{')
                            nested_level++;
                        else if (json_str[pos] == '}')
                            nested_level--;
                        pos++;
                    }

                    if (nested_level != 0)
                        throw std::runtime_error("Unterminated object at position " +
                                                 std::to_string(value_start));

                    result[key] = json_str.substr(value_start, pos - value_start);
                }
                else if (std::isdigit(json_str[pos]) || json_str[pos] == '-')
                {
                    // 数字值 - 直接解析并存储为字符串
                    int number_value = parse_number();
                    result[key] = std::to_string(number_value);
                }
                else
                {
                    throw std::runtime_error("Unexpected character at position " +
                                             std::to_string(pos));
                }

                skip_whitespace();
                if (json_str[pos] == '}')
                {
                    pos++; // 跳过 '}'
                    break;
                }

                if (json_str[pos] != ',')
                    throw std::runtime_error("Expected ',' or '}' at position " +
                                             std::to_string(pos));

                pos++; // 跳过 ','
            }

            return result;
        }

      public:
        parser(std::string_view json_string) : json_str(json_string)
        {
        }

        std::unordered_map<std::string, std::string> parse()
        {
            skip_whitespace();
            auto result = parse_object();
            skip_whitespace();

            if (pos < json_str.size())
                throw std::runtime_error("Unexpected data after JSON object");

            return result;
        }
    };

    // 从 JSON 字符串反序列化到聚合类型
    template <MeaningfulAggregate T> static T from_json(std::string_view json_str)
    {
        parser p(json_str);
        auto parsed_data = p.parse();

        T result{};

        visit_each_member(
            result,
            [&parsed_data]<class M, int index>(auto&& member)
            {
                using MemberType = std::decay_t<decltype(member)>;

                // 获取成员名称
                std::string member_name(get_member_names<M>()[index]);

                if (parsed_data.find(member_name) == parsed_data.end())
                    return; // 字段不存在，跳过

                std::string value = parsed_data[member_name];

                if constexpr (std::is_same_v<MemberType, std::string>)
                {
                    // 字符串类型 - 如果有引号则去除
                    if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
                        value = value.substr(1, value.size() - 2);
                    member = value;
                }
                else if constexpr (std::is_integral_v<MemberType>)
                {
                    // 整数类型 - 清除可能的引号
                    if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
                        value = value.substr(1, value.size() - 2);
                    member = std::stoi(value);
                }
                else if constexpr (std::is_floating_point_v<MemberType>)
                {
                    // 浮点数类型 - 清除可能的引号
                    if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
                        value = value.substr(1, value.size() - 2);
                    member = std::stof(value);
                }
                else if constexpr (MeaningfulAggregate<MemberType>)
                {
                    // 递归处理嵌套聚合类型
                    member = from_json<MemberType>(value);
                }
            });

        return result;
    }

    // 为了方便使用的辅助函数
    template <typename T> static T parse(const std::string& json_str)
    {
        return from_json<T>(json_str);
    }
};
