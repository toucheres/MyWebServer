#pragma once
#include "mysqlHandle.h"
#include "reflection.hpp"
#include "string"
#include <functional>
#include <optional>
#include <sstream>
#include <string_view>
#include <vector>

template <const char* attributes, class type> class with_attributes
{
};

// SQL基础类型
struct only_content
{
    std::string content;

    // 添加隐式转换操作符
    operator std::string() const
    {
        return content;
    }

    // 添加赋值操作符
    only_content& operator=(const std::string& str)
    {
        content = str;
        return *this;
    }
    only_content& operator=(std::string&& str)
    {
        content = std::move(str);
        return *this;
    }
    
    // 添加字符串字面量赋值操作符
    only_content& operator=(const char* str)
    {
        content = str;
        return *this;
    }
    
    // 添加构造函数支持
    only_content() = default;
    only_content(const std::string& str) : content(str) {}
    only_content(std::string&& str) : content(std::move(str)) {}
    only_content(const char* str) : content(str) {}
};

template <size_t size> struct varchar : only_content
{
    // 继承基类的构造函数
    using only_content::only_content;
    using only_content::operator=;
    
    static constexpr std::string_view sql_type()
    {
        return "VARCHAR(" + std::to_string(size) + ")";
    }
    static constexpr size_t max_length = size;
};
struct text : only_content
{
    // 继承基类的构造函数
    using only_content::only_content;
    using only_content::operator=;
    
    static constexpr std::string_view sql_type()
    {
        return "TEXT";
    }
};
struct bigint : only_content
{
    // 继承基类的构造函数和赋值操作符
    using only_content::only_content;
    using only_content::operator=;
    
    // 添加数值类型的构造函数
    bigint(long long val) : only_content(std::to_string(val)) {}
    bigint(int val) : only_content(std::to_string(val)) {}
    
    static constexpr std::string_view sql_type()
    {
        return "BIGINT";
    }

    // 添加数值转换
    operator long long() const
    {
        return std::stoll(content);
    }
    bigint& operator=(long long val)
    {
        content = std::to_string(val);
        return *this;
    }
    bigint& operator=(int val)
    {
        content = std::to_string(val);
        return *this;
    }
};
struct timestamp : only_content
{
    // 继承基类的构造函数
    using only_content::only_content;
    using only_content::operator=;
    
    static constexpr std::string_view sql_type()
    {
        return "TIMESTAMP";
    }
};

// 增强的类型萃取工具 - 修复对包装类型的递归解析
template <typename T> struct type_unwrapper
{
    using type = T;
    inline static constexpr bool nullable = []() -> bool
    {
        // 检查类型是否可以为空值
        if constexpr (std::is_pointer_v<T>)
        {
            return true;
        }
        else if constexpr (requires { typename T::nullable_tag; })
        {
            return true;
        }
        else
        {
            return false;
        }
    }();
    inline static constexpr std::string_view Key = "";
    inline static constexpr std::string_view Default = "";
    inline static constexpr std::string_view Extra = "";
};

template <typename T> class data_views
{
  public:
    T data;

    data_views(const T& d) : data(d)
    {
    }

    // 将数据转换为结构体
    constexpr auto to_struct() const
    {
        return data;
    }

    // 生成INSERT SQL语句 - 改进版本，表名默认为类名
    std::string to_insert_sql(const std::string& table_name = "", bool skip_auto_fields = true) const
    {
        std::string sql = "INSERT INTO ";
        // 如果没有提供表名，则使用类名作为默认表名
        std::string actual_table_name = table_name.empty() ? std::string(get_class_name<T>()) : table_name;
        sql += actual_table_name;
        sql += " (";

        // 添加字段名，跳过自增ID和空的timestamp字段
        constexpr auto names = get_member_names<T>();
        std::vector<std::string> valid_fields;
        std::vector<size_t> valid_indices;
        
        for (size_t i = 0; i < names.size(); ++i)
        {
            // 检查是否应该跳过此字段
            bool should_skip = false;
            
            if (skip_auto_fields) {
                // 跳过第一个字段（通常是自增ID）
                if (i == 0) {
                    should_skip = true;
                }
                // 检查timestamp字段是否为空
                else {
                    visit_each_member(data, [&, i]<class M, int index>(auto&& member) {
                        if (index == i) {
                            using MemberType = std::decay_t<decltype(member)>;
                            if constexpr (requires { member.content; }) {
                                if (member.content.empty()) {
                                    should_skip = true;
                                }
                            }
                        }
                    });
                }
            }
            
            if (!should_skip) {
                valid_fields.push_back(std::string(names[i]));
                valid_indices.push_back(i);
            }
        }
        
        // 构建字段列表
        for (size_t i = 0; i < valid_fields.size(); ++i)
        {
            if (i > 0) sql += ", ";
            sql += valid_fields[i];
        }
        sql += ") VALUES (";

        // 添加占位符
        for (size_t i = 0; i < valid_fields.size(); ++i)
        {
            if (i > 0) sql += ", ";
            sql += "?";
        }
        sql += ")";
        return sql;
    }

    // 生成UPDATE SQL语句 - 改进版本，表名默认为类名
    std::string to_update_sql(const std::string& where_clause,
                              const std::string& table_name = "",
                              bool skip_auto_fields = true) const
    {
        std::string sql = "UPDATE ";
        // 如果没有提供表名，则使用类名作为默认表名
        std::string actual_table_name = table_name.empty() ? std::string(get_class_name<T>()) : table_name;
        sql += actual_table_name;
        sql += " SET ";

        // 获取有效字段，跳过自增ID和空的timestamp字段
        constexpr auto names = get_member_names<T>();
        std::vector<std::string> valid_fields;
        
        for (size_t i = 0; i < names.size(); ++i)
        {
            // 检查是否应该跳过此字段
            bool should_skip = false;
            
            if (skip_auto_fields) {
                // 跳过第一个字段（通常是自增ID）
                if (i == 0) {
                    should_skip = true;
                }
                // 对于UPDATE，不跳过timestamp字段，因为可能需要更新
            }
            
            if (!should_skip) {
                valid_fields.push_back(std::string(names[i]));
            }
        }
        
        // 构建SET子句
        for (size_t i = 0; i < valid_fields.size(); ++i)
        {
            if (i > 0) sql += ", ";
            sql += valid_fields[i] + " = ?";
        }

        if (!where_clause.empty())
        {
            sql += " WHERE " + where_clause;
        }

        return sql;
    }

    // 绑定参数到prepared statement - 改进版本
    bool bind_to_statement(MySQLStatementPtr stmt, bool skip_auto_fields = true) const
    {
        if (!stmt)
            return false;

        size_t param_index = 0;
        bool success = true;

        visit_each_member(data,
                          [&stmt, &param_index, &success, skip_auto_fields]<class M, int index>(auto&& member)
                          {
                              using MemberType = std::decay_t<decltype(member)>;

                              // 检查是否应该跳过此字段
                              bool should_skip = false;
                              
                              if (skip_auto_fields) {
                                  // 跳过第一个字段（通常是自增ID）
                                  if (index == 0) {
                                      should_skip = true;
                                  }
                                  // 检查timestamp字段是否为空
                                  else if constexpr (requires { member.content; }) {
                                      if (member.content.empty()) {
                                          should_skip = true;
                                      }
                                  }
                              }
                              
                              if (should_skip) {
                                  return; // 跳过此字段，不增加param_index
                              }

                              if constexpr (std::is_same_v<MemberType, std::string>)
                              {
                                  success &= stmt->setString(param_index, member);
                              }
                              else if constexpr (std::is_same_v<MemberType, int>)
                              {
                                  success &= stmt->setInt(param_index, member);
                              }
                              else if constexpr (std::is_floating_point_v<MemberType>)
                              {
                                  success &=
                                      stmt->setDouble(param_index, static_cast<double>(member));
                              }
                              else if constexpr (requires { member.content; })
                              {
                                  success &= stmt->setString(param_index, member.content);
                              }
                              else
                              {
                                  success &= stmt->setString(param_index, std::to_string(member));
                              }

                              param_index++;
                          });

        return success;
    }

    // 为UPDATE操作绑定参数
    bool bind_to_update_statement(MySQLStatementPtr stmt, bool skip_auto_fields = true) const
    {
        if (!stmt)
            return false;

        size_t param_index = 0;
        bool success = true;

        visit_each_member(data,
                          [&stmt, &param_index, &success, skip_auto_fields]<class M, int index>(auto&& member)
                          {
                              using MemberType = std::decay_t<decltype(member)>;

                              // 检查是否应该跳过此字段
                              bool should_skip = false;
                              
                              if (skip_auto_fields) {
                                  // 跳过第一个字段（通常是自增ID）
                                  if (index == 0) {
                                      should_skip = true;
                                  }
                                  // 对于UPDATE，不跳过timestamp字段
                              }
                              
                              if (should_skip) {
                                  return; // 跳过此字段，不增加param_index
                              }

                              if constexpr (std::is_same_v<MemberType, std::string>)
                              {
                                  success &= stmt->setString(param_index, member);
                              }
                              else if constexpr (std::is_same_v<MemberType, int>)
                              {
                                  success &= stmt->setInt(param_index, member);
                              }
                              else if constexpr (std::is_floating_point_v<MemberType>)
                              {
                                  success &=
                                      stmt->setDouble(param_index, static_cast<double>(member));
                              }
                              else if constexpr (requires { member.content; })
                              {
                                  success &= stmt->setString(param_index, member.content);
                              }
                              else
                              {
                                  success &= stmt->setString(param_index, std::to_string(member));
                              }

                              param_index++;
                          });

        return success;
    }

    // 从MySQLResult构造对象
    static std::optional<data_views<T>> from_result_row(const std::vector<std::string>& row)
    {
        if (row.empty())
            return std::nullopt;

        T obj{};
        size_t field_index = 0;

        visit_each_member(obj,
                          [&row, &field_index]<class M, int index>(auto&& member)
                          {
                              if (field_index < row.size())
                              {
                                  using MemberType = std::decay_t<decltype(member)>;

                                  const std::string& value = row[field_index];

                                  if (value == "NULL")
                                  {
                                      // 处理NULL值，跳过赋值
                                  }
                                  else if constexpr (std::is_same_v<MemberType, std::string>)
                                  {
                                      member = value;
                                  }
                                  else if constexpr (std::is_same_v<MemberType, int>)
                                  {
                                      member = std::stoi(value);
                                  }
                                  else if constexpr (std::is_floating_point_v<MemberType>)
                                  {
                                      member = std::stof(value);
                                  }
                                  else if constexpr (requires { member.content; })
                                  {
                                      member.content = value;
                                  }

                                  field_index++;
                              }
                          });

        return data_views<T>(obj);
    }
};

template <class T> class enable_sql
{
  private:
    MySQLHandle& handle;
    std::string table_name_override;

  public:
    enable_sql(MySQLHandle& h) : handle(h)
    {
    }
    enable_sql(MySQLHandle& h, const std::string& table_name)
        : handle(h), table_name_override(table_name)
    {
    }

    // 获取表名
    std::string table_name() const
    {
        return table_name_override.empty() ? std::string(get_class_name<T>()) : table_name_override;
    }

    // 将结构体转换为data_views
    data_views<T> struct_to_data_views(const T& obj) const
    {
        return data_views<T>(obj);
    }

    // 插入单个对象 - 改进版本
    bool insert(const T& obj) const
    {
        auto dv = struct_to_data_views(obj);
        std::string sql = dv.to_insert_sql(table_name(), true); // 启用自动字段跳过

        auto stmt = handle.prepareStatement(sql);
        if (!stmt)
        {
            std::cerr << "Failed to prepare insert statement: " << handle.getLastError()
                      << std::endl;
            return false;
        }

        if (!dv.bind_to_statement(stmt, true)) // 启用自动字段跳过
        {
            std::cerr << "Failed to bind parameters" << std::endl;
            return false;
        }

        auto result = stmt->execute();
        return result && result->getAffectedRows() > 0;
    }

    // 批量插入
    bool insert_batch(const std::vector<T>& objects) const
    {
        if (objects.empty())
            return true;

        bool success = handle.startTransaction();
        if (!success)
        {
            std::cerr << "Failed to start transaction" << std::endl;
            return false;
        }

        try
        {
            for (const auto& obj : objects)
            {
                if (!insert(obj))
                {
                    handle.rollback();
                    return false;
                }
            }

            return handle.commit();
        }
        catch (...)
        {
            handle.rollback();
            return false;
        }
    }

    // 更新对象 - 改进版本
    bool update(const T& obj, const std::string& where_clause) const
    {
        auto dv = struct_to_data_views(obj);
        std::string sql = dv.to_update_sql(where_clause, table_name(), true); // 启用自动字段跳过

        auto stmt = handle.prepareStatement(sql);
        if (!stmt)
        {
            std::cerr << "Failed to prepare update statement: " << handle.getLastError()
                      << std::endl;
            return false;
        }

        if (!dv.bind_to_update_statement(stmt, true)) // 使用专门的UPDATE绑定方法
        {
            std::cerr << "Failed to bind parameters" << std::endl;
            return false;
        }

        auto result = stmt->execute();
        return result && result->getAffectedRows() > 0;
    }

    // 删除对象
    bool remove(const std::string& where_clause) const
    {
        std::string sql = "DELETE FROM " + table_name();
        if (!where_clause.empty())
        {
            sql += " WHERE " + where_clause;
        }

        auto result = handle.query(sql);
        return result && result->getAffectedRows() > 0;
    }

    // 查询所有对象
    std::vector<data_views<T>> select_all() const
    {
        return select_where("");
    }

    // 条件查询
    std::vector<data_views<T>> select_where(const std::string& where_clause) const
    {
        std::vector<data_views<T>> results;

        std::string sql = "SELECT * FROM " + table_name();
        if (!where_clause.empty())
        {
            sql += " WHERE " + where_clause;
        }

        auto result = handle.query(sql);
        if (!result || result->empty())
        {
            return results;
        }

        // 获取所有行
        auto rows = result->getRows();
        for (const auto& row : rows)
        {
            auto dv = data_views<T>::from_result_row(row);
            if (dv)
            {
                results.push_back(*dv);
            }
        }

        return results;
    }

    // 查询单个对象
    std::optional<data_views<T>> select_one(const std::string& where_clause) const
    {
        auto results = select_where(where_clause + " LIMIT 1");
        return results.empty() ? std::nullopt : std::make_optional(results[0]);
    }

    // 统计记录数
    size_t count(const std::string& where_clause = "") const
    {
        std::string sql = "SELECT COUNT(*) FROM " + table_name();
        if (!where_clause.empty())
        {
            sql += " WHERE " + where_clause;
        }

        auto result = handle.query(sql);
        if (!result || result->empty())
        {
            return 0;
        }

        auto row = result->getRow();
        return row.empty() ? 0 : std::stoull(row[0]);
    }

    // 检查表是否存在
    bool table_exists() const
    {
        std::string sql = "SHOW TABLES LIKE '" + table_name() + "'";
        auto result = handle.query(sql);
        return result && !result->empty();
    }

    // 创建表
    bool create_table() const
    {
        std::string sql = generate_create_table_sql();
        auto result = handle.query(sql);
        return result != nullptr;
    }

    // 删除表
    bool drop_table() const
    {
        std::string sql = "DROP TABLE IF EXISTS " + table_name();
        auto result = handle.query(sql);
        return result != nullptr;
    }

    // 生成创建表的SQL语句
    std::string generate_create_table_sql() const
    {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + table_name() + " (\n";

        constexpr auto names = get_member_names<T>();
        constexpr auto types = get_member_class_names<T>();

        for (size_t i = 0; i < names.size(); ++i)
        {
            if (i > 0)
                sql += ",\n";
            sql += "    " + std::string(names[i]) + " ";

            // 映射C++类型到SQL类型
            std::string_view cpp_type = types[i];
            if (cpp_type == "int")
            {
                sql += "INT";
            }
            else if (cpp_type == "string")
            {
                sql += "VARCHAR(255)";
            }
            else if (cpp_type.find("varchar") != std::string_view::npos)
            {
                // 从varchar<N>中提取大小
                sql += "VARCHAR(255)"; // 默认大小，实际应该解析模板参数
            }
            else if (cpp_type == "bigint")
            {
                sql += "BIGINT";
            }
            else if (cpp_type == "text")
            {
                sql += "TEXT";
            }
            else if (cpp_type == "timestamp")
            {
                sql += "TIMESTAMP DEFAULT CURRENT_TIMESTAMP";
            }
            else
            {
                sql += "TEXT"; // 默认类型
            }

            // 添加约束
            if (i == 0)
            { // 假设第一个字段是主键
                sql += " AUTO_INCREMENT PRIMARY KEY";
            }
            else
            {
                sql += " NOT NULL";
            }
        }

        sql += "\n)";
        return sql;
    }

    // 获取最后插入的ID
    long long get_last_insert_id() const
    {
        auto result = handle.query("SELECT LAST_INSERT_ID()");
        if (!result || result->empty())
        {
            return 0;
        }

        auto row = result->getRow();
        return row.empty() ? 0 : std::stoll(row[0]);
    }
};

// 辅助函数：创建enable_sql实例
template <class T> auto make_sql_enabled(MySQLHandle& handle)
{
    return enable_sql<T>(handle);
}

template <class T> auto make_sql_enabled(MySQLHandle& handle, const std::string& table_name)
{
    return enable_sql<T>(handle, table_name);
}

// 便利宏：快速定义SQL操作类
#define ENABLE_SQL_OPS(Type, handle_ref)                                                           \
    static auto sql_ops()                                                                          \
    {                                                                                              \
        return make_sql_enabled<Type>(handle_ref);                                                 \
    }