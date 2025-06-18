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

    // 添加配置选项
    struct options
    {
        bool flatten_single_member;
    };
    static inline options defultOpt = options{
        .flatten_single_member = false,
    };

    json() = default;
    template <Aggregate T> static json from(T in, options opts = defultOpt)
    {
        return json(in, opts, true); // 明确指定为根级别
    }
    static json from(const std::string& in)
    {
        json j;
        j.content = in;
        return j;
    }

    template <Aggregate T> static T to(json in, options opts = defultOpt)
    {
        return from_json<T>(in.content, opts);
    }
    template <Aggregate T> static T to(std::string in, options opts = defultOpt)
    {
        json j;
        j.content = in;
        return from_json<T>(j.content, opts);
    }
    operator std::string()
    {
        return content;
    }

  private:
    friend std::ostream& operator<<(std::ostream& os, const json& j)
    {
        return os << j.content;
    }
    // 修改构造函数，添加选项参数
    template <Aggregate T> json(T in, options opts = {}, bool is_root = true)
    {
        // 检查类型是否只有一个成员
        constexpr size_t member_count = get_member_count_v<T>;

        // 只有在非根级别且启用扁平化且只有一个成员时才进行扁平化
        if (opts.flatten_single_member && member_count == 1 && !is_root)
        {
            // 单成员情况：直接序列化该成员的值
            visit_each_member(in,
                              [this, opts]<class M, int index>(auto&& arg)
                              {
                                  using ArgType = std::decay_t<decltype(arg)>;

                                  if constexpr (requires { (std::string) arg; })
                                  {
                                      content = "\"" + (std::string)arg + "\"";
                                  }
                                  else if constexpr (std::is_integral_v<ArgType> ||
                                                     std::is_floating_point_v<ArgType>)
                                  {
                                      content = std::to_string(arg);
                                  }
                                  else if constexpr (requires { std::to_string(arg); })
                                  {
                                      content = "\"" + std::to_string(arg) + "\"";
                                  }
                                  else if constexpr (MeaningfulAggregate<ArgType>)
                                  {
                                      // 递归处理嵌套聚合类型，传递 is_root = false
                                      json nested(arg, opts, false);
                                      content = nested.content;
                                  }
                              });
        }
        else
        {
            // 正常情况：按原逻辑处理
            serialize_normal(in, opts);
        }
    }

    // 将原有的序列化逻辑提取为单独方法
    template <Aggregate T> void serialize_normal(T in, options opts)
    {
        content += "{";
        visit_each_member(
            in,
            [this, opts]<class M, int index>(auto&& arg)
            {
                using ArgType = std::decay_t<decltype(arg)>;

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
                    add_number(get_member_names<M>()[index], std::to_string(arg));
                }
                else if constexpr (requires { std::to_string(arg); })
                {
                    add_string(get_member_names<M>()[index], std::to_string(arg));
                }
                else if constexpr (MeaningfulAggregate<ArgType>)
                {
                    // 递归处理嵌套聚合类型，传递 is_root = false
                    json nested(arg, opts, false);
                    add_object(get_member_names<M>()[index], nested.content);
                }
                content += ',';
            });
        if (!content.empty() && content.back() == ',')
            content.pop_back();
        content += "}";
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
                    size_t obj_start = pos; // 记录对象开始位置
                    pos++;                  // 跳过 '{'

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

                    // 包含整个对象的JSON字符串，包括花括号
                    result[key] = json_str.substr(obj_start, pos - obj_start);
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

            // 检查是否是对象开始
            if (pos < json_str.size() && json_str[pos] == '{')
            {
                auto result = parse_object();
                skip_whitespace();

                if (pos < json_str.size())
                    throw std::runtime_error("Unexpected data after JSON object");

                return result;
            }
            // 处理直接值（扁平化情况）
            else if (pos < json_str.size())
            {
                std::string direct_value;
                if (json_str[pos] == '"')
                {
                    direct_value = parse_string();
                }
                else if (std::isdigit(json_str[pos]) || json_str[pos] == '-')
                {
                    direct_value = std::to_string(parse_number());
                }
                else
                {
                    throw std::runtime_error("Unexpected character at position " +
                                             std::to_string(pos));
                }

                // 返回一个特殊标记的映射，表示这是一个直接值
                std::unordered_map<std::string, std::string> result;
                result["__direct_value__"] = direct_value;
                return result;
            }

            throw std::runtime_error("Empty JSON input");
        }
    };

    // 在现有 from_json 函数之外添加此重载
    template <MeaningfulAggregate T>
    static T from_json(const std::string& json_str, options opts = defultOpt)
    {
        return from_json<T>(std::string_view(json_str), opts);
    }

    // 尝试将JSON值解析为特定类型
    template <typename T> static bool try_parse_value(const std::string& value, T& result)
    {
        try
        {
            if constexpr (std::is_same_v<T, std::string>)
            {
                // 字符串类型 - 如果有引号则去除
                std::string temp = value;
                if (temp.size() >= 2 && temp.front() == '"' && temp.back() == '"')
                    temp = temp.substr(1, temp.size() - 2);
                result = temp;
                return true;
            }
            else if constexpr (std::is_integral_v<T>)
            {
                // 整数类型 - 清除可能的引号
                std::string temp = value;
                if (temp.size() >= 2 && temp.front() == '"' && temp.back() == '"')
                    temp = temp.substr(1, temp.size() - 2);
                result = std::stoi(temp);
                return true;
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                // 浮点数类型 - 清除可能的引号
                std::string temp = value;
                if (temp.size() >= 2 && temp.front() == '"' && temp.back() == '"')
                    temp = temp.substr(1, temp.size() - 2);
                result = std::stof(temp);
                return true;
            }
            // else if constexpr (requires { result.content; })
            // {
            //     // 处理带有content成员的类型
            //     std::string temp = value;
            //     if (temp.size() >= 2 && temp.front() == '"' && temp.back() == '"')
            //         temp = temp.substr(1, temp.size() - 2);
            //     result.content = temp;
            //     return true;
            // }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    // 从 JSON 字符串反序列化到聚合类型
    template <MeaningfulAggregate T>
    static T from_json(std::string_view json_str, options opts = defultOpt)
    {
        parser p(json_str);
        auto parsed_data = p.parse();

        T result{};

        visit_each_member(
            result,
            [&parsed_data, &opts]<class M, int index>(auto&& member)
            {
                using MemberType = std::decay_t<decltype(member)>;

                // 获取成员名称
                std::string member_name(get_member_names<M>()[index]);

                if (parsed_data.find(member_name) == parsed_data.end())
                    return; // 字段不存在，跳过

                std::string value = parsed_data[member_name];

                try
                {
                    if constexpr (std::is_same_v<MemberType, std::string>)
                    {
                        // 字符串类型处理
                        if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
                            value = value.substr(1, value.size() - 2);
                        member = value;
                    }
                    else if constexpr (std::is_integral_v<MemberType>)
                    {
                        // 整数类型处理
                        if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
                            value = value.substr(1, value.size() - 2);
                        member = std::stoi(value);
                    }
                    else if constexpr (MeaningfulAggregate<MemberType>)
                    {
                        if (opts.flatten_single_member)
                        {
                            // 处理扁平化模式下的嵌套结构体
                            constexpr size_t nested_member_count = get_member_count_v<MemberType>;
                            if (nested_member_count == 1)
                            {
                                // 直接将值赋给嵌套结构体的唯一成员
                                visit_each_member(
                                    member,
                                    [&value]<class NestedM, int nested_index>(auto&& nested_member)
                                    {
                                        using NestedType = std::decay_t<decltype(nested_member)>;

                                        // 去除引号
                                        if (value.size() >= 2 && value.front() == '"' &&
                                            value.back() == '"')
                                            value = value.substr(1, value.size() - 2);

                                        if constexpr (std::is_same_v<NestedType, std::string>)
                                        {
                                            nested_member = value;
                                        }
                                        else if constexpr (std::is_integral_v<NestedType>)
                                        {
                                            nested_member = std::stoi(value);
                                        }
                                    });
                                // 已处理完毕，跳过后续递归处理
                                return;
                            }
                        }

                        // 递归处理普通嵌套聚合类型
                        member = from_json<MemberType>(value, opts);
                    }
                }
                catch (...)
                {
                    // 处理解析异常
                }
            });

        return result;
    }
};