#pragma once
#include "reflection_ylt.hpp"
#include <array>
#include <charconv>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

// 容器类型检测模板（移到全局作用域）
template <typename T> struct is_vector : std::false_type
{
};

template <typename T, typename Alloc> struct is_vector<std::vector<T, Alloc>> : std::true_type
{
};

template <typename T> struct is_list : std::false_type
{
};

template <typename T, typename Alloc> struct is_list<std::list<T, Alloc>> : std::true_type
{
};

// 添加双端队列类型检测
template <typename T> struct is_deque : std::false_type
{
};

template <typename T, typename Alloc> struct is_deque<std::deque<T, Alloc>> : std::true_type
{
};

template <typename T> struct is_map : std::false_type
{
};

template <typename K, typename V, typename Compare, typename Alloc>
struct is_map<std::map<K, V, Compare, Alloc>> : std::true_type
{
};

template <typename K, typename V, typename Hash, typename Equal, typename Alloc>
struct is_map<std::unordered_map<K, V, Hash, Equal, Alloc>> : std::true_type
{
};

// 添加array类型检测
template <typename T> struct is_array : std::false_type
{
};

template <typename T, std::size_t N> struct is_array<std::array<T, N>> : std::true_type
{
};

// 修改set类型检测 - 只检测普通set
template <typename T> struct is_set : std::false_type
{
};

template <typename T, typename Compare, typename Alloc>
struct is_set<std::set<T, Compare, Alloc>> : std::true_type
{
};

// 添加多重集合类型检测
template <typename T> struct is_multiset : std::false_type
{
};

template <typename T, typename Compare, typename Alloc>
struct is_multiset<std::multiset<T, Compare, Alloc>> : std::true_type
{
};

// 修改无序集合类型检测 - 只检测unordered_set
template <typename T> struct is_unordered_set : std::false_type
{
};

template <typename T, typename Hash, typename Equal, typename Alloc>
struct is_unordered_set<std::unordered_set<T, Hash, Equal, Alloc>> : std::true_type
{
};

template <typename T> static constexpr bool is_vector_v = is_vector<T>::value;

template <typename T> static constexpr bool is_list_v = is_list<T>::value;

template <typename T> static constexpr bool is_deque_v = is_deque<T>::value;

template <typename T> static constexpr bool is_map_v = is_map<T>::value;

template <typename T> static constexpr bool is_array_v = is_array<T>::value;

template <typename T> static constexpr bool is_set_v = is_set<T>::value;

template <typename T> static constexpr bool is_multiset_v = is_multiset<T>::value;

template <typename T> static constexpr bool is_unordered_set_v = is_unordered_set<T>::value;

// 更新容器分类
template <typename T>
static constexpr bool is_sequence_container_v = 
    is_vector_v<T> || is_list_v<T> || is_array_v<T> || is_deque_v<T>;

template <typename T>
static constexpr bool is_set_like_v = 
    is_set_v<T> || is_multiset_v<T> || is_unordered_set_v<T>;

template <typename T>
static constexpr bool is_container_v = is_sequence_container_v<T> || is_set_like_v<T> || is_map_v<T>;

// concept声明移到全局作用域
template <typename T>
concept reflectable =
    Aggregate<T> || is_container_v<T>;

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
    template <reflectable T> static json from(T in, options opts = defultOpt)
    {
        return json(in, opts, true); // 明确指定为根级别
    }
    static json from(const std::string& in)
    {
        json j;
        j.content = in;
        return j;
    }

    template <reflectable T> static T to(json in, options opts = defultOpt)
    {
        return from_json<T>(in.content, opts);
    }
    template <reflectable T> static T to(std::string in, options opts = defultOpt)
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

    // 修改构造函数，添加选项参数 - 聚合类型
    template <Aggregate T>
        requires(!is_array_v<T>) // 排除std::array，避免重载歧义
    json(T in, options opts = {}, bool is_root = true)
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
                                      content = "\"" + escape_string((std::string)arg) + "\"";
                                  }
                                  else if constexpr (std::is_integral_v<ArgType> ||
                                                     std::is_floating_point_v<ArgType>)
                                  {
                                      content = std::to_string(arg);
                                  }
                                  else if constexpr (requires { std::to_string(arg); })
                                  {
                                      content = "\"" + escape_string(std::to_string(arg)) + "\"";
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

    // 修改容器类型的构造函数
    template <typename T>
        requires(is_sequence_container_v<T> || is_set_like_v<T>)
    json(T in, options opts = {}, bool = true) // 移除参数名避免警告
    {
        content = serialize_array(in, opts);
    }

    // 添加map类型的构造函数
    template <typename T>
        requires is_map_v<T>
    json(T in, options opts = {}, bool = true) // 移除参数名避免警告
    {
        content = serialize_map(in, opts);
    }

    // 将原有的序列化逻辑提取为单独方法
    template <reflectable T> void serialize_normal(T in, options opts)
    {
        content += "{";
        visit_each_member(
            in,
            [this, opts]<class M, int index>(auto&& arg)
            {
                using ArgType = std::decay_t<decltype(arg)>;

                static_assert(
                    requires { (std::string) arg; } || requires { std::to_string(arg); } ||
                        MeaningfulAggregate<ArgType> || is_container_v<ArgType>,
                    "不支持的成员类型：必须是可转为字符串、有 std::to_string "
                    "支持、嵌套聚合类或容器类");

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
                else if constexpr (is_sequence_container_v<ArgType> || is_set_like_v<ArgType>)
                {
                    // 处理所有序列容器和集合类容器
                    std::string array_content = serialize_array(arg, opts);
                    add_array(get_member_names<M>()[index], array_content);
                }
                else if constexpr (is_map_v<ArgType>)
                {
                    // 处理map
                    std::string map_content = serialize_map(arg, opts);
                    add_object(get_member_names<M>()[index], map_content);
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

    // 序列化数组容器
    template <typename Container>
    std::string serialize_array(const Container& container, options opts)
    {
        std::string result = "[";
        bool first = true;

        for (const auto& item : container)
        {
            if (!first)
                result += ",";
            first = false;

            using ItemType = std::decay_t<decltype(item)>;

            if constexpr (requires { (std::string) item; })
            {
                result += "\"" + escape_string((std::string)item) + "\"";
            }
            else if constexpr (std::is_integral_v<ItemType> || std::is_floating_point_v<ItemType>)
            {
                result += std::to_string(item);
            }
            else if constexpr (requires { std::to_string(item); })
            {
                result += "\"" + escape_string(std::to_string(item)) + "\"";
            }
            else if constexpr (MeaningfulAggregate<ItemType>)
            {
                json nested(item, opts, false);
                result += nested.content;
            }
            else if constexpr (is_container_v<ItemType>)
            {
                if constexpr (is_sequence_container_v<ItemType> || is_set_like_v<ItemType>)
                {
                    result += serialize_array(item, opts);
                }
                else if constexpr (is_map_v<ItemType>)
                {
                    result += serialize_map(item, opts);
                }
            }
        }

        result += "]";
        return result;
    }

    // 序列化map容器
    template <typename MapType> std::string serialize_map(const MapType& map, options opts)
    {
        std::string result = "{";
        bool first = true;

        for (const auto& [key, value] : map)
        {
            if (!first)
                result += ",";
            first = false;

            // 键必须是字符串类型
            std::string key_str;
            if constexpr (requires { (std::string) key; })
            {
                key_str = (std::string)key;
            }
            else if constexpr (requires { std::to_string(key); })
            {
                key_str = std::to_string(key);
            }

            result += "\"" + escape_string(key_str) + "\":";

            using ValueType = std::decay_t<decltype(value)>;

            if constexpr (requires { (std::string) value; })
            {
                result += "\"" + escape_string((std::string)value) + "\"";
            }
            else if constexpr (std::is_integral_v<ValueType> || std::is_floating_point_v<ValueType>)
            {
                result += std::to_string(value);
            }
            else if constexpr (requires { std::to_string(value); })
            {
                result += "\"" + escape_string(std::to_string(value)) + "\"";
            }
            else if constexpr (MeaningfulAggregate<ValueType>)
            {
                json nested(value, opts, false);
                result += nested.content;
            }
            else if constexpr (is_container_v<ValueType>)
            {
                if constexpr (is_sequence_container_v<ValueType> || is_set_like_v<ValueType>)
                {
                    result += serialize_array(value, opts);
                }
                else if constexpr (is_map_v<ValueType>)
                {
                    result += serialize_map(value, opts);
                }
            }
        }

        result += "}";
        return result;
    }

    // 添加字符串转义函数
    static std::string escape_string(std::string_view str)
    {
        std::string result;
        result.reserve(str.size() * 2); // 预留空间，避免频繁重分配

        for (char c : str)
        {
            switch (c)
            {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += c;
                break;
            }
        }
        return result;
    }

    // 修改add_string方法使用转义函数
    void add_string(std::string_view key, std::string_view value)
    {
        content += "\"";
        content += key;
        content += "\":\"";
        content += escape_string(value);
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

    // 添加一个处理数组的方法
    void add_array(std::string_view key, std::string_view value)
    {
        content += "\"";
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

        std::string parse_array()
        {
            if (json_str[pos] != '[')
                throw std::runtime_error("Expected '[' at position " + std::to_string(pos));

            size_t array_start = pos;
            int bracket_level = 1;
            pos++; // 跳过 '['

            while (bracket_level > 0 && pos < json_str.size())
            {
                if (json_str[pos] == '[')
                    bracket_level++;
                else if (json_str[pos] == ']')
                    bracket_level--;
                pos++;
            }

            if (bracket_level != 0)
                throw std::runtime_error("Unterminated array at position " +
                                         std::to_string(array_start));

            return std::string(json_str.substr(array_start, pos - array_start));
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
                else if (json_str[pos] == '[')
                {
                    // 数组值 - 存储为原始JSON字符串
                    result[key] = parse_array();
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
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    // 重新组织 from_json 函数重载，避免歧义
    
    // 1. std::array 特化 - 最高优先级
    template <typename T>
        requires is_array_v<T>
    static T from_json(const std::string& json_str, options opts = defultOpt)
    {
        return parse_array_to_container<T>(json_str, opts);
    }

    template <typename T>
        requires is_array_v<T>
    static T from_json(std::string_view json_str, options opts = defultOpt)
    {
        return parse_array_to_container<T>(json_str, opts);
    }

    // 2. map 容器 - 第二优先级
    template <typename T>
        requires is_map_v<T>
    static T from_json(const std::string& json_str, options opts = defultOpt)
    {
        return from_json<T>(std::string_view(json_str), opts);
    }

    template <typename T>
        requires is_map_v<T>
    static T from_json(std::string_view json_str, options opts = defultOpt)
    {
        T result;
        parser p(json_str);
        auto parsed_data = p.parse();

        for (const auto& [key, val] : parsed_data)
        {
            using KeyType = typename T::key_type;
            using ValueType = typename T::mapped_type;

            KeyType parsed_key{};
            ValueType parsed_value{};

            if (try_parse_value(key, parsed_key) &&
                try_parse_container_item(val, parsed_value, opts))
            {
                result[parsed_key] = parsed_value;
            }
        }

        return result;
    }

    // 3. 序列容器（除了array） - 第三优先级
    template <typename T>
        requires(is_sequence_container_v<T> && !is_array_v<T>)
    static T from_json(const std::string& json_str, options opts = defultOpt)
    {
        return parse_array_to_container<T>(json_str, opts);
    }

    template <typename T>
        requires(is_sequence_container_v<T> && !is_array_v<T>)
    static T from_json(std::string_view json_str, options opts = defultOpt)
    {
        return parse_array_to_container<T>(json_str, opts);
    }

    // 4. 集合类容器 - 第四优先级
    template <typename T>
        requires is_set_like_v<T>
    static T from_json(const std::string& json_str, options opts = defultOpt)
    {
        return parse_array_to_container<T>(json_str, opts);
    }

    template <typename T>
        requires is_set_like_v<T>
    static T from_json(std::string_view json_str, options opts = defultOpt)
    {
        return parse_array_to_container<T>(json_str, opts);
    }

    // 5. 聚合类型的反序列化 - 最低优先级
    template <MeaningfulAggregate T>
        requires(!is_container_v<T>)
    static T from_json(const std::string& json_str, options opts = defultOpt)
    {
        return from_json<T>(std::string_view(json_str), opts);
    }

    template <MeaningfulAggregate T>
        requires(!is_container_v<T>)
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
                    else if constexpr (is_sequence_container_v<MemberType> || is_set_like_v<MemberType>)
                    {
                        // 处理所有序列容器和集合类容器
                        member = parse_array_to_container<MemberType>(value, opts);
                    }
                    else if constexpr (is_map_v<MemberType>)
                    {
                        // 处理map容器
                        parser nested_parser(value);
                        auto nested_data = nested_parser.parse();

                        for (const auto& [key, val] : nested_data)
                        {
                            using KeyType = typename MemberType::key_type;
                            using ValueType = typename MemberType::mapped_type;

                            KeyType parsed_key{};
                            ValueType parsed_value{};

                            if (try_parse_value(key, parsed_key) &&
                                try_parse_container_item(val, parsed_value, opts))
                            {
                                member[parsed_key] = parsed_value;
                            }
                        }
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

    // 解析数组内容到容器
    template <typename Container>
    static Container parse_array_to_container(std::string_view array_str, options opts = defultOpt)
    {
        Container result;

        if (array_str.size() < 2 || array_str.front() != '[' || array_str.back() != ']')
            return result;

        // 去除方括号
        array_str = array_str.substr(1, array_str.size() - 2);

        // 简单的逗号分割（这里可以优化为更完整的JSON解析）
        size_t pos = 0;
        std::string current_item;
        int bracket_level = 0;
        int brace_level = 0;
        bool in_string = false;
        bool escaped = false;
        size_t item_index = 0;

        while (pos < array_str.size())
        {
            char c = array_str[pos];

            if (!escaped && c == '"' && bracket_level == 0 && brace_level == 0)
            {
                in_string = !in_string;
            }
            else if (!in_string)
            {
                if (c == '[')
                    bracket_level++;
                else if (c == ']')
                    bracket_level--;
                else if (c == '{')
                    brace_level++;
                else if (c == '}')
                    brace_level--;
                else if (c == ',' && bracket_level == 0 && brace_level == 0)
                {
                    // 找到一个完整的项
                    std::string trimmed = current_item;
                    // 去除前后空格
                    size_t start = trimmed.find_first_not_of(" \t\n\r");
                    size_t end = trimmed.find_last_not_of(" \t\n\r");
                    if (start != std::string::npos)
                    {
                        trimmed = trimmed.substr(start, end - start + 1);

                        using ValueType = typename Container::value_type;
                        ValueType item{};
                        if (try_parse_container_item(trimmed, item, opts))
                        {
                            if constexpr (requires { result.push_back(item); })
                            {
                                result.push_back(std::move(item));
                            }
                            else if constexpr (is_array_v<Container>)
                            {
                                // 对于std::array，需要使用索引赋值
                                if (item_index < result.size())
                                {
                                    result[item_index] = std::move(item);
                                }
                            }
                            else if constexpr (is_set_like_v<Container>)
                            {
                                // 对于所有集合类型，使用insert
                                result.insert(std::move(item));
                            }
                        }
                    }
                    current_item.clear();
                    item_index++;
                    pos++;
                    continue;
                }
            }

            escaped = (!escaped && c == '\\');
            current_item += c;
            pos++;
        }

        // 处理最后一个项
        if (!current_item.empty())
        {
            std::string trimmed = current_item;
            size_t start = trimmed.find_first_not_of(" \t\n\r");
            size_t end = trimmed.find_last_not_of(" \t\n\r");
            if (start != std::string::npos)
            {
                trimmed = trimmed.substr(start, end - start + 1);

                using ValueType = typename Container::value_type;
                ValueType item{};
                if (try_parse_container_item(trimmed, item, opts))
                {
                    if constexpr (requires { result.push_back(item); })
                    {
                        result.push_back(std::move(item));
                    }
                    else if constexpr (is_array_v<Container>)
                    {
                        // 对于std::array，需要使用索引赋值
                        if (item_index < result.size())
                        {
                            result[item_index] = std::move(item);
                        }
                    }
                    else if constexpr (is_set_like_v<Container>)
                    {
                        // 对于所有集合类型，使用insert
                        result.insert(std::move(item));
                    }
                }
            }
        }

        return result;
    }

    // 尝试解析容器项
    template <typename T>
    static bool try_parse_container_item(const std::string& value, T& result, options opts)
    {
        try
        {
            if constexpr (std::is_same_v<T, std::string>)
            {
                std::string temp = value;
                if (temp.size() >= 2 && temp.front() == '"' && temp.back() == '"')
                    temp = temp.substr(1, temp.size() - 2);
                result = temp;
                return true;
            }
            else if constexpr (std::is_integral_v<T>)
            {
                std::string temp = value;
                if (temp.size() >= 2 && temp.front() == '"' && temp.back() == '"')
                    temp = temp.substr(1, temp.size() - 2);
                result = std::stoi(temp);
                return true;
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                std::string temp = value;
                if (temp.size() >= 2 && temp.front() == '"' && temp.back() == '"')
                    temp = temp.substr(1, temp.size() - 2);
                result = std::stof(temp);
                return true;
            }
            else if constexpr (MeaningfulAggregate<T>)
            {
                result = from_json<T>(value, opts);
                return true;
            }
            else if constexpr (is_container_v<T>)
            {
                result = from_json<T>(value, opts);
                return true;
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }
};