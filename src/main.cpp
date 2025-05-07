#include <format.h>
#include <iostream>
#include <string>
#include <variant>

// 用于打印变体类型的辅助函数
void printVariant(const std::variant<int, double, std::string>& var) {
    if (std::holds_alternative<int>(var)) {
        std::cout << std::get<int>(var);
    } else if (std::holds_alternative<double>(var)) {
        std::cout << std::get<double>(var);
    } else if (std::holds_alternative<std::string>(var)) {
        std::cout << "\"" << std::get<std::string>(var) << "\"";
    }
}

int main()
{
    // 测试不同类型的Format匹配
    Format exact{"hello", Format::Type::same};
    Format prefix{"pre", Format::Type::prefix};
    Format suffix{"fix", Format::Type::suffix};
    Format regex{"(\\d+)-(\\w+)", Format::Type::regex};
    Format scanf{"Value: %d, Name: %s", Format::Type::scanf};

    // 测试基本匹配功能
    std::cout << "基本匹配测试:\n";
    std::cout << "Exact match: " << std::boolalpha << exact.match("hello") << std::endl;
    std::cout << "Prefix match: " << prefix.match("prefix") << std::endl;
    std::cout << "Suffix match: " << suffix.match("suffix") << std::endl;
    std::cout << "Regex match: " << regex.match("123-abc") << std::endl;
    std::cout << "Scanf match: " << scanf.match("Value: 42, Name: John") << std::endl;
    
    // 测试解析功能
    std::cout << "\n解析功能测试:\n";
    
    // 测试正则表达式解析
    std::string regexTest = "123-abc";
    auto regexResult = regex.parse(regexTest);
    std::cout << "Regex parse \"" << regexTest << "\": ";
    if (regexResult) {
        std::cout << "成功，捕获组: ";
        for (const auto& val : *regexResult) {
            printVariant(val);
            std::cout << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "失败" << std::endl;
    }
    
    // 测试scanf解析
    std::string scanfTest = "Value: 42, Name: John";
    auto scanfResult = scanf.parse(scanfTest);
    std::cout << "Scanf parse \"" << scanfTest << "\": ";
    if (scanfResult) {
        std::cout << "成功，变量: ";
        for (const auto& val : *scanfResult) {
            printVariant(val);
            std::cout << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "失败" << std::endl;
    }
    
    return 0;
}