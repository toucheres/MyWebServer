#pragma once
#include "mysqlHandle.h"
#include "reflection_ylt.hpp"
#include "string"
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

template <class type> class table_discribe;
struct describer
{
    friend table_discribe<class type>;
    std::string primerkey;
    std::string table_name;
    std::vector<std::string> field_names;
    std::vector<std::string> field_types;
    std::vector<std::string> field_constraints;
    std::vector<bool> skip_on_insert;
    std::vector<bool> skip_on_update;
    bool auto_timestamp = true;

    // 辅助方法
    size_t field_count() const
    {
        return field_names.size();
    }

    bool should_skip_field_on_insert(size_t index) const
    {
        return index < skip_on_insert.size() ? skip_on_insert[index] : false;
    }

    bool should_skip_field_on_update(size_t index) const
    {
        return index < skip_on_update.size() ? skip_on_update[index] : false;
    }

    std::string get_field_type(size_t index) const
    {
        return index < field_types.size() ? field_types[index] : "TEXT";
    }

    std::string get_field_constraint(size_t index) const
    {
        return index < field_constraints.size() ? field_constraints[index] : "NOT NULL";
    }
};

template <class type> class table_discribe
{
  public:
    static describer description;

    // 获取表名
    static std::string get_table_name()
    {
        if (!description.table_name.empty())
        {
            return description.table_name;
        }
        return std::string(get_class_name<type>());
    }

    // 获取主键字段名
    static std::string get_primary_key()
    {
        return description.primerkey;
    }

    // 初始化描述信息的静态方法
    static void init_description(const describer& desc)
    {
        description = desc;
    }

    // 自动从反射信息生成描述
    static void auto_generate_description()
    {
        if (!description.field_names.empty())
        {
            return; // 已经初始化过了
        }

        description.table_name = std::string(get_class_name<type>());

        constexpr auto names = get_member_names<type>();
        constexpr auto types = get_member_class_names<type>();

        description.field_names.clear();
        description.field_types.clear();
        description.field_constraints.clear();
        description.skip_on_insert.clear();
        description.skip_on_update.clear();

        // 预留容量
        description.field_names.reserve(names.size());
        description.field_types.reserve(names.size());
        description.field_constraints.reserve(names.size());
        description.skip_on_insert.reserve(names.size());
        description.skip_on_update.reserve(names.size());

        for (size_t i = 0; i < names.size(); ++i)
        {
            description.field_names.emplace_back(names[i]);

            // 映射类型名到SQL类型
            std::string_view cpp_type = types[i];
            std::string sql_type;
            std::string constraint;
            bool skip_insert = false;
            bool skip_update = false;

            // 更精确的类型映射
            if (cpp_type == "int")
            {
                sql_type = "INT";
            }
            else if (cpp_type.starts_with("varchar"))
            {
                // 从varchar<N>中提取大小
                if (auto pos = cpp_type.find('<'); pos != std::string_view::npos)
                {
                    auto end_pos = cpp_type.find('>', pos);
                    if (end_pos != std::string_view::npos)
                    {
                        auto size_str = cpp_type.substr(pos + 1, end_pos - pos - 1);
                        sql_type = "VARCHAR(" + std::string(size_str) + ")";
                    }
                    else
                    {
                        sql_type = "VARCHAR(255)";
                    }
                }
                else
                {
                    sql_type = "VARCHAR(255)";
                }
            }
            else if (cpp_type == "text")
            {
                sql_type = "TEXT";
            }
            else if (cpp_type == "bigint")
            {
                sql_type = "BIGINT";
            }
            else if (cpp_type == "timestamp")
            {
                sql_type = "TIMESTAMP";
                constraint = "DEFAULT CURRENT_TIMESTAMP";
                skip_insert = true;
            }
            else
            {
                sql_type = "TEXT";
            }

            // 第一个字段默认为主键
            if (i == 0)
            {
                if (constraint.empty())
                {
                    constraint = "AUTO_INCREMENT PRIMARY KEY";
                }
                else
                {
                    constraint += " AUTO_INCREMENT PRIMARY KEY";
                }
                skip_insert = true;
                skip_update = true;
                if (description.primerkey.empty())
                {
                    description.primerkey = std::string(names[i]);
                }
            }
            else if (constraint.empty())
            {
                constraint = "NOT NULL";
            }

            description.field_types.push_back(sql_type);
            description.field_constraints.push_back(constraint);
            description.skip_on_insert.push_back(skip_insert);
            description.skip_on_update.push_back(skip_update);
        }
    }

    // 获取描述信息的访问器
    static const describer& get_description()
    {
        // 如果描述信息为空，自动生成
        if (description.field_names.empty())
        {
            auto_generate_description();
        }
        return description;
    }
};

// 静态成员定义
template <class type> describer table_discribe<type>::description = {};

// 移除only_content结构体，并将类型定义修改为聚合类型
template <size_t size> struct varchar
{
    std::string content;

    // // 转换操作符
    // operator std::string() const
    // {
    //     return content;
    // }

    // 赋值操作符
    varchar& operator=(const std::string& str)
    {
        content = str;
        return *this;
    }

    varchar& operator=(std::string&& str)
    {
        content = std::move(str);
        return *this;
    }

    varchar& operator=(const char* str)
    {
        content = str;
        return *this;
    }
    bool operator==(const auto& str)
    {
        return content == str;
    }
    bool operator!=(const char* str)
    {
        return !(content == str);
    }
    static constexpr const char* sql_type()
    {
        if constexpr (size <= 50)
        {
            return "VARCHAR(50)";
        }
        else if constexpr (size <= 100)
        {
            return "VARCHAR(100)";
        }
        else if constexpr (size <= 255)
        {
            return "VARCHAR(255)";
        }
        else if constexpr (size <= 500)
        {
            return "VARCHAR(500)";
        }
        else if constexpr (size <= 1000)
        {
            return "VARCHAR(1000)";
        }
        else
        {
            return "TEXT";
        }
    }
    static constexpr size_t max_length = size;
};

struct text
{
    std::string content;

    // // 转换操作符
    // operator std::string() const
    // {
    //     return content;
    // }

    // 赋值操作符
    text& operator=(const std::string& str)
    {
        content = str;
        return *this;
    }

    text& operator=(std::string&& str)
    {
        content = std::move(str);
        return *this;
    }

    text& operator=(const char* str)
    {
        content = str;
        return *this;
    }

    static constexpr const char* sql_type()
    {
        return "TEXT";
    }
};

struct bigint
{
    std::string content;

    // // 转换操作符
    // operator std::string() const
    // {
    //     return content;
    // }

    operator long long() const
    {
        return std::stoll(content);
    }

    // 赋值操作符
    bigint& operator=(const std::string& str)
    {
        content = str;
        return *this;
    }

    bigint& operator=(std::string&& str)
    {
        content = std::move(str);
        return *this;
    }

    bigint& operator=(const char* str)
    {
        content = str;
        return *this;
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

    static constexpr const char* sql_type()
    {
        return "BIGINT";
    }
};

struct timestamp
{
    std::string content;

    // // 转换操作符
    // operator std::string() const
    // {
    //     return content;
    // }

    // 赋值操作符
    timestamp& operator=(const std::string& str)
    {
        content = str;
        return *this;
    }

    timestamp& operator=(std::string&& str)
    {
        content = std::move(str);
        return *this;
    }

    timestamp& operator=(const char* str)
    {
        content = str;
        return *this;
    }

    static constexpr const char* sql_type()
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
    std::string to_insert_sql(const std::string& table_name = "",
                              bool skip_auto_fields = true) const
    {
        std::string sql = "INSERT INTO ";
        // 如果没有提供表名，则使用类名作为默认表名
        std::string actual_table_name =
            table_name.empty() ? std::string(get_class_name<T>()) : table_name;
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

            if (skip_auto_fields)
            {
                // 跳过第一个字段（通常是自增ID）
                if (i == 0)
                {
                    should_skip = true;
                }
                // 检查timestamp字段是否为空
                else
                {
                    visit_each_member(data,
                                      [&, i]<class M, int index>(auto&& member)
                                      {
                                          if (index == i)
                                          {
                                              // using MemberType = std::decay_t<decltype(member)>;
                                              if constexpr (requires { member.content; })
                                              {
                                                  if (member.content.empty())
                                                  {
                                                      should_skip = true;
                                                  }
                                              }
                                          }
                                      });
                }
            }

            if (!should_skip)
            {
                valid_fields.push_back(std::string(names[i]));
                valid_indices.push_back(i);
            }
        }

        // 构建字段列表
        for (size_t i = 0; i < valid_fields.size(); ++i)
        {
            if (i > 0)
                sql += ", ";
            sql += valid_fields[i];
        }
        sql += ") VALUES (";

        // 添加占位符
        for (size_t i = 0; i < valid_fields.size(); ++i)
        {
            if (i > 0)
                sql += ", ";
            sql += "?";
        }
        sql += ")";
        return sql;
    }

    // 生成UPDATE SQL语句 - 改进版本，表名默认为类名
    std::string to_update_sql(const std::string& where_clause, const std::string& table_name = "",
                              bool skip_auto_fields = true) const
    {
        std::string sql = "UPDATE ";
        // 如果没有提供表名，则使用类名作为默认表名
        std::string actual_table_name =
            table_name.empty() ? std::string(get_class_name<T>()) : table_name;
        sql += actual_table_name;
        sql += " SET ";

        // 获取有效字段，跳过自增ID和空的timestamp字段
        constexpr auto names = get_member_names<T>();
        std::vector<std::string> valid_fields;

        for (size_t i = 0; i < names.size(); ++i)
        {
            // 检查是否应该跳过此字段
            bool should_skip = false;

            if (skip_auto_fields)
            {
                // 跳过第一个字段（通常是自增ID）
                if (i == 0)
                {
                    should_skip = true;
                }
                // 对于UPDATE，不跳过timestamp字段，因为可能需要更新
            }

            if (!should_skip)
            {
                valid_fields.push_back(std::string(names[i]));
            }
        }

        // 构建SET子句
        for (size_t i = 0; i < valid_fields.size(); ++i)
        {
            if (i > 0)
                sql += ", ";
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

        visit_each_member(
            data,
            [&stmt, &param_index, &success, skip_auto_fields]<class M, int index>(auto&& member)
            {
                using MemberType = std::decay_t<decltype(member)>;

                // 检查是否应该跳过此字段
                bool should_skip = false;

                if (skip_auto_fields)
                {
                    // 跳过第一个字段（通常是自增ID）
                    if (index == 0)
                    {
                        should_skip = true;
                    }
                    // 检查timestamp字段是否为空
                    else if constexpr (requires { member.content; })
                    {
                        if (member.content.empty())
                        {
                            should_skip = true;
                        }
                    }
                }

                if (should_skip)
                {
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
                    success &= stmt->setDouble(param_index, static_cast<double>(member));
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

        visit_each_member(
            data,
            [&stmt, &param_index, &success, skip_auto_fields]<class M, int index>(auto&& member)
            {
                using MemberType = std::decay_t<decltype(member)>;

                // 检查是否应该跳过此字段
                bool should_skip = false;

                if (skip_auto_fields)
                {
                    // 跳过第一个字段（通常是自增ID）
                    if (index == 0)
                    {
                        should_skip = true;
                    }
                    // 对于UPDATE，不跳过timestamp字段
                }

                if (should_skip)
                {
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
                    success &= stmt->setDouble(param_index, static_cast<double>(member));
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

    // 获取表名 - 从table_discribe获取
    std::string table_name() const
    {
        if (!table_name_override.empty())
        {
            return table_name_override;
        }
        return table_discribe<T>::get_table_name();
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

    // 生成创建表的SQL语句 - 使用描述信息
    std::string generate_create_table_sql() const
    {
        const auto& desc = table_discribe<T>::get_description();
        std::string sql = "CREATE TABLE IF NOT EXISTS " + table_name() + " (\n";

        for (size_t i = 0; i < desc.field_count(); ++i)
        {
            if (i > 0)
                sql += ",\n";
            sql += "    " + desc.field_names[i] + " " + desc.field_types[i] + " " +
                   desc.field_constraints[i];
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

// 便利宏：定义表描述信息
#define DEFINE_TABLE_DESCRIPTION(Type, TableName, PrimaryKey, ...)                                 \
    template <>                                                                                    \
    describer table_discribe<Type>::description = {                                                \
        .primerkey = PrimaryKey, .table_name = TableName, __VA_ARGS__};