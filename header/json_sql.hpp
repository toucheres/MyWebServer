#pragma once
#include "json.hpp"
#include "reflection.hpp"
#include <iostream>
#include <regex>
#include <string>
#include <vector>

// JSON<->SQL 转换器类
class JsonSql 
{
public:
    // 从聚合类型生成 SQL INSERT 语句
    template <Aggregate T>
    static std::string toInsertSql(const T& data, const std::string& table_name) 
    {
        std::string columns = "(";
        std::string values = "(";
        
        visit_each_member(data, 
            [&columns, &values]<class M, int index>(auto&& member) {
                using MemberType = std::decay_t<decltype(member)>;
                std::string member_name(get_member_names<M>()[index]);
                
                // 跳过自增ID字段
                if (member_name == "id" || member_name == "created_at") {
                    return;
                }
                
                columns += member_name + ", ";
                
                if constexpr (std::is_same_v<MemberType, std::string>) {
                    values += "'" + member + "', ";
                } 
                else if constexpr (std::is_integral_v<MemberType> || std::is_floating_point_v<MemberType>) {
                    values += std::to_string(member) + ", ";
                }
                else if constexpr (MeaningfulAggregate<MemberType>) {
                    // 复杂类型转为JSON字符串
                    values += "'" + json::from(member).content + "', ";
                }
            });
        
        // 移除末尾的逗号和空格
        if (columns.size() > 1) {
            columns.erase(columns.size() - 2);
            values.erase(values.size() - 2);
        }
        
        columns += ")";
        values += ")";
        
        return "INSERT INTO " + table_name + " " + columns + " VALUES " + values;
    }
    
    // 从SQL语句解析到聚合类型，使用模板字符串指定要提取的值
    template <Aggregate T>
    static T fromInsertSql(const std::string& sql, const std::string& template_str) 
    {
        T result{};
        
        // 使用正则表达式提取值部分
        std::regex values_regex(template_str);
        std::smatch matches;
        
        if (std::regex_search(sql, matches, values_regex)) {
            if (matches.size() >= 5) {  // 确保捕获了两个字段和两个值
                // 创建字段到值的映射
                std::map<std::string, std::string> field_values;
                
                // matches[1]和matches[2]是字段名，matches[3]和matches[5]是对应的值
                std::string field1 = matches[1].str();
                std::string field2 = matches[2].str();
                std::string value1 = matches[3].str();
                std::string value2 = matches[5].str();
                
                // 去除可能的空白
                field1.erase(0, field1.find_first_not_of(" \t\n\r"));
                field1.erase(field1.find_last_not_of(" \t\n\r") + 1);
                field2.erase(0, field2.find_first_not_of(" \t\n\r"));
                field2.erase(field2.find_last_not_of(" \t\n\r") + 1);
                
                field_values[field1] = value1;
                field_values[field2] = value2;
                
                // 按字段名映射填充对象
                visit_each_member(result, 
                    [&field_values]<class M, int index>(auto&& member) {
                        std::string member_name(get_member_names<M>()[index]);
                        if (field_values.find(member_name) != field_values.end()) {
                            assignValue(member, field_values[member_name]);
                        }
                    });
            } else {
                // 兼容旧的处理方式，按顺序填充（但可能出错）
                std::vector<std::string> extracted_values;
                for (size_t i = 1; i < matches.size(); ++i) {
                    extracted_values.push_back(matches[i].str());
                }
                fillObject(result, extracted_values);
            }
        }
        
        return result;
    }
    
    // 从 SQL UPDATE 语句中提取值
    template <Aggregate T>
    static T fromUpdateSql(const std::string& sql) 
    {
        T result{};
        std::regex set_regex("SET\\s+([^;]+)(?:WHERE|$)");
        std::smatch matches;
        
        if (std::regex_search(sql, matches, set_regex) && matches.size() > 1) {
            std::string set_clause = matches[1];
            std::regex assignment_regex("([^=\\s]+)\\s*=\\s*['\"]?([^'\"\\s,]+)['\"]?\\s*,?");
            
            std::string::const_iterator searchStart(set_clause.cbegin());
            std::map<std::string, std::string> field_values;
            
            while (std::regex_search(searchStart, set_clause.cend(), matches, assignment_regex)) {
                if (matches.size() > 2) {
                    std::string field = matches[1];
                    std::string value = matches[2];
                    field_values[field] = value;
                }
                searchStart = matches.suffix().first;
            }
            
            // 填充对象
            visit_each_member(result, 
                [&field_values]<class M, int index>(auto&& member) {
                    std::string member_name(get_member_names<M>()[index]);
                    if (field_values.find(member_name) != field_values.end()) {
                        assignValue(member, field_values[member_name]);
                    }
                });
        }
        
        return result;
    }
    
    // 生成 SQL UPDATE 语句
    template <Aggregate T>
    static std::string toUpdateSql(const T& data, const std::string& table_name, const std::string& where_clause) 
    {
        std::string set_clause = "";
        
        visit_each_member(data, 
            [&set_clause]<class M, int index>(auto&& member) {
                using MemberType = std::decay_t<decltype(member)>;
                std::string member_name(get_member_names<M>()[index]);
                
                // 跳过ID和created_at字段
                if (member_name == "id" || member_name == "created_at") {
                    return;
                }
                
                set_clause += member_name + " = ";
                
                if constexpr (std::is_same_v<MemberType, std::string>) {
                    set_clause += "'" + member + "'";
                } 
                else if constexpr (std::is_integral_v<MemberType> || std::is_floating_point_v<MemberType>) {
                    set_clause += std::to_string(member);
                }
                else if constexpr (MeaningfulAggregate<MemberType>) {
                    set_clause += "'" + json::from(member).content + "'";
                }
                
                set_clause += ", ";
            });
        
        // 移除末尾的逗号和空格
        if (!set_clause.empty()) {
            set_clause.erase(set_clause.size() - 2);
        }
        
        return "UPDATE " + table_name + " SET " + set_clause + " WHERE " + where_clause;
    }

private:
    // 根据提取的值填充对象
    template <Aggregate T>
    static void fillObject(T& obj, const std::vector<std::string>& values) 
    {
        size_t current_index = 0;
        
        visit_each_member(obj, 
            [&values, &current_index]<class M, int index>(auto&& member) {
                if (current_index < values.size()) {
                    assignValue(member, values[current_index++]);
                }
            });
    }
    
    // 根据类型将字符串值分配给成员
    template <typename T>
    static void assignValue(T& member, const std::string& value) 
    {
        if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
            // 去除可能的引号
            if (value.size() >= 2 && (value.front() == '\'' || value.front() == '"') && 
                (value.back() == '\'' || value.back() == '"')) {
                member = value.substr(1, value.size() - 2);
            } else {
                member = value;
            }
        }
        else if constexpr (std::is_integral_v<std::decay_t<T>>) {
            try {
                // 检查字符串是否为空或仅包含空格
                if (value.empty() || value.find_first_not_of(" \t\n\r") == std::string::npos) {
                    member = 0; // 设置默认值
                    return;
                }
                
                // 处理可能的引号
                std::string clean_value = value;
                if (clean_value.size() >= 2 && 
                    (clean_value.front() == '\'' || clean_value.front() == '"') && 
                    (clean_value.back() == '\'' || clean_value.back() == '"')) {
                    clean_value = clean_value.substr(1, clean_value.size() - 2);
                }
                
                // 尝试转换
                size_t pos;
                member = static_cast<std::decay_t<T>>(std::stoll(clean_value, &pos));
                
                // 检查是否完整转换
                if (pos != clean_value.size()) {
                    std::cerr << "警告: 字符串 \"" << clean_value 
                              << "\" 包含非数字字符, 只解析了前 " << pos << " 个字符" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "转换错误: 无法将 \"" << value << "\" 转换为整数: " << e.what() << std::endl;
                member = 0; // 设置默认值
            }
        }
        else if constexpr (std::is_floating_point_v<std::decay_t<T>>) {
            try {
                // 检查字符串是否为空或仅包含空格
                if (value.empty() || value.find_first_not_of(" \t\n\r") == std::string::npos) {
                    member = 0.0; // 设置默认值
                    return;
                }
                
                // 处理可能的引号
                std::string clean_value = value;
                if (clean_value.size() >= 2 && 
                    (clean_value.front() == '\'' || clean_value.front() == '"') && 
                    (clean_value.back() == '\'' || clean_value.back() == '"')) {
                    clean_value = clean_value.substr(1, clean_value.size() - 2);
                }
                
                member = std::stof(clean_value);
            } catch (const std::exception& e) {
                std::cerr << "转换错误: 无法将 \"" << value << "\" 转换为浮点数: " << e.what() << std::endl;
                member = 0.0; // 设置默认值
            }
        }
        else if constexpr (MeaningfulAggregate<std::decay_t<T>>) {
            try {
                member = json::parse<std::decay_t<T>>(value);
            } catch (const std::exception& e) {
                std::cerr << "解析错误: 无法解析JSON字符串: " << e.what() << std::endl;
            }
        }
    }
};
