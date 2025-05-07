#include <string>
// #include <system_error>
#include <vector>
#include <variant>
#include <optional>

class Format
{
  public:
    enum class Type
    {
        prefix,
        suffix,
        same,
        regex,
        scanf  // 添加scanf类型
    };
    
    // 用于存储解析结果的类型
    using ParseResult = std::vector<std::variant<int, double, std::string>>;
    
    Format(std::string format, Type type);
    Format() = default;
    Format(const Format& format) = default;
    Format(Format&& format) = default;
    
    // 添加字符串匹配方法
    bool match(const std::string& str) const;
    
    // 添加解析方法，返回解析结果
    std::optional<ParseResult> parse(const std::string& str) const;
    
    // 添加与字符串直接比较的方法
    bool operator==(const std::string& str) const;
    
    // 为兼容性保留的操作符
    bool operator==(const Format& in) const;
    bool operator<(const Format& in) const;

    std::string format = "";

  private:
    Format::Type type;
    
    // 工具方法：使用scanf格式解析字符串
    std::optional<ParseResult> parseScanf(const std::string& str) const;
    
    // 工具方法：使用正则表达式解析字符串
    std::optional<ParseResult> parseRegex(const std::string& str) const;
};
