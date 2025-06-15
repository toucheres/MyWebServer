// mysql> show tables;
// +-------------------+
// | Tables_in_chat_db |
// +-------------------+
// | messages          |
// | users             |
// +-------------------+
// 2 rows in set (0.00 sec)

// mysql> describe messages;
// +------------+-------------+------+-----+-------------------+-------------------+
// | Field      | Type        | Null | Key | Default           | Extra             |
// +------------+-------------+------+-----+-------------------+-------------------+
// | id         | int         | NO   | PRI | NULL              | auto_increment    |
// | sender     | varchar(50) | NO   |     | NULL              |                   |
// | content    | text        | NO   |     | NULL              |                   |
// | created_at | timestamp   | YES  |     | CURRENT_TIMESTAMP | DEFAULT_GENERATED |
// +------------+-------------+------+-----+-------------------+-------------------+
// 5 rows in set (0.01 sec)
// +----+--------------+--------------------------------------------------------------------------------------------------------+------------+---------------------+
// | id | sender       | content | timestamp  | created_at          |
// +----+--------------+--------------------------------------------------------------------------------------------------------+------------+---------------------+
// |  1 | root         | sdf | 1747462695 | 2025-05-17 14:18:15 |

// mysql> describe users;
// +------------+--------------+------+-----+-------------------+-------------------+
// | Field      | Type         | Null | Key | Default           | Extra             |
// +------------+--------------+------+-----+-------------------+-------------------+
// | id         | int          | NO   | PRI | NULL              | auto_increment    |
// | username   | varchar(50)  | NO   | UNI | NULL              |                   |
// | password   | varchar(100) | NO   |     | NULL              |                   |
// | created_at | timestamp    | YES  |     | CURRENT_TIMESTAMP | DEFAULT_GENERATED |
// +------------+--------------+------+-----+-------------------+-------------------+
// 4 rows in set (0.00 sec)
// +----+----------------------------+-------------+---------------------+
// | id | username                   | password    | created_at          |
// +----+----------------------------+-------------+---------------------+
// |  1 | root                       | 000000      | 2025-05-17 14:18:13 |
// mysql>

#include "corutine.hpp"
#include "file.h"
#include "http.h"
#include "json_sql.hpp"
#include "mysqlHandle.h"
#include "signal_slots.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

// 定义与数据库表对应的结构体
struct users
{
    int id;
    varchar<50> username;
    varchar<100> password;
    timestamp created_at;
};

struct messages
{
    int id;
    varchar<50> sender;
    text content;
    timestamp created_at;
};

// 定义表描述信息
template <>
describer table_discribe<users>::description = {
    .primerkey = "id",
    .table_name = "users",
    .field_names = {"id", "username", "password", "created_at"},
    .field_types = {"INT", "VARCHAR(50)", "VARCHAR(100)", "TIMESTAMP DEFAULT CURRENT_TIMESTAMP"},
    .field_constraints = {"AUTO_INCREMENT PRIMARY KEY", "NOT NULL UNIQUE", "NOT NULL",
                          "DEFAULT_GENERATED"},
    .skip_on_insert = {true, false, false, true}, // 跳过id和created_at
    .skip_on_update = {true, false, false, true}, // 跳过id
    .auto_timestamp = true};

template <>
describer table_discribe<messages>::description = {
    .primerkey = "id",
    .table_name = "messages",
    .field_names = {"id", "sender", "content", "created_at"},
    .field_types = {"INT", "VARCHAR(50)", "TEXT", "TIMESTAMP DEFAULT CURRENT_TIMESTAMP"},
    .field_constraints = {"AUTO_INCREMENT PRIMARY KEY", "NOT NULL", "NOT NULL",
                          "DEFAULT_GENERATED"},
    .skip_on_insert = {true, false, false, true}, // 跳过id和created_at
    .skip_on_update = {true, false, false, true}, // 跳过id
    .auto_timestamp = true};

class stdiolistener : public co_async, public enable_auto_remove_slots
{
  public:
    signal<std::string_view> on_user_input;
    virtual EventStatus eventGo()
    {
        auto& stdaio = async_in_out::getInstance();
        auto in = stdaio.read_line();

        if (in == std::string{""})
        {
            return EventStatus::Continue;
        }
        else if (in == std::string{"quit\n"})
        {
            on_exit.emit();
            return EventStatus::Stop;
        }
        else
        {
            on_user_input.emit(in);
            return EventStatus::Continue;
        }
    }
};
HttpServer server;
MySQLHandle sql{"localhost", "webserver", "WebServer@2025", "chat_db"};

void test_table_description()
{
    std::cout << "\n=== 表描述信息测试 ===" << std::endl;

    // 测试users表描述
    std::cout << "\n1. Users表描述信息:" << std::endl;
    const auto& users_desc = table_discribe<users>::get_description();
    std::cout << "表名: " << users_desc.table_name << std::endl;
    std::cout << "主键: " << users_desc.primerkey << std::endl;
    std::cout << "字段数量: " << users_desc.field_count() << std::endl;

    for (size_t i = 0; i < users_desc.field_count(); ++i)
    {
        std::cout << "字段 " << i << ": " << users_desc.field_names[i] << " "
                  << users_desc.field_types[i] << " " << users_desc.field_constraints[i]
                  << " (插入时跳过: " << users_desc.should_skip_field_on_insert(i)
                  << ", 更新时跳过: " << users_desc.should_skip_field_on_update(i) << ")"
                  << std::endl;
    }

    // 测试messages表描述
    std::cout << "\n2. Messages表描述信息:" << std::endl;
    const auto& messages_desc = table_discribe<messages>::get_description();
    std::cout << "表名: " << messages_desc.table_name << std::endl;
    std::cout << "主键: " << messages_desc.primerkey << std::endl;
    std::cout << "字段数量: " << messages_desc.field_count() << std::endl;

    for (size_t i = 0; i < messages_desc.field_count(); ++i)
    {
        std::cout << "字段 " << i << ": " << messages_desc.field_names[i] << " "
                  << messages_desc.field_types[i] << " " << messages_desc.field_constraints[i]
                  << " (插入时跳过: " << messages_desc.should_skip_field_on_insert(i)
                  << ", 更新时跳过: " << messages_desc.should_skip_field_on_update(i) << ")"
                  << std::endl;
    }

    // 测试自动生成表名
    std::cout << "\n3. 从table_discribe获取表名:" << std::endl;
    std::cout << "Users表名: " << table_discribe<users>::get_table_name() << std::endl;
    std::cout << "Messages表名: " << table_discribe<messages>::get_table_name() << std::endl;
    std::cout << "Users主键: " << table_discribe<users>::get_primary_key() << std::endl;
    std::cout << "Messages主键: " << table_discribe<messages>::get_primary_key() << std::endl;
}

void test_json_sql_operations()
{
    std::cout << "=== JSON-SQL 操作示例 (使用表描述信息) ===" << std::endl;

    // 创建 SQL 操作实例 - 不指定表名，将从table_discribe获取
    auto user_ops = make_sql_enabled<users>(sql);
    auto message_ops = make_sql_enabled<messages>(sql);

    std::cout << "\n1. 从表描述获取的表名:" << std::endl;
    std::cout << "Users table name: " << user_ops.table_name() << std::endl;
    std::cout << "Messages table name: " << message_ops.table_name() << std::endl;

    std::cout << "\n2. 检查表是否存在:" << std::endl;
    std::cout << "Users table exists: " << user_ops.table_exists() << std::endl;
    std::cout << "Messages table exists: " << message_ops.table_exists() << std::endl;

    // 生成并显示CREATE TABLE语句
    std::cout << "\n3. 根据表描述生成CREATE TABLE语句:" << std::endl;
    std::cout << "Users CREATE TABLE:\n" << user_ops.generate_create_table_sql() << std::endl;
    std::cout << "\nMessages CREATE TABLE:\n"
              << message_ops.generate_create_table_sql() << std::endl;

    // 创建测试用户
    users test_user;
    test_user.id = 0; // 根据描述信息，插入时会被跳过
    test_user.username = "desc_test_user";
    test_user.password = "password123";
    // created_at根据描述信息在插入时会被跳过

    std::cout << "\n4. 使用表描述信息插入测试用户:" << std::endl;
    if (user_ops.insert(test_user))
    {
        std::cout << "用户插入成功，ID: " << user_ops.get_last_insert_id() << std::endl;
    }
    else
    {
        std::cout << "用户插入失败" << std::endl;
    }

    // 查询用户
    std::cout << "\n5. 查询用户:" << std::endl;
    auto users_result = user_ops.select_where("username = 'desc_test_user'");
    for (const auto& user_view : users_result)
    {
        const auto& user = user_view.data;
        std::cout << "ID: " << user.id << ", 用户名: " << user.username.content
                  << ", 密码: " << user.password.content
                  << ", 创建时间: " << user.created_at.content << std::endl;
    }

    // 创建测试消息
    messages test_message;
    test_message.id = 0; // 根据描述信息，插入时会被跳过
    test_message.sender = "desc_test_user";
    test_message.content = "这是使用表描述信息的测试消息";
    // created_at根据描述信息在插入时会被跳过

    std::cout << "\n6. 使用表描述信息插入测试消息:" << std::endl;
    if (message_ops.insert(test_message))
    {
        std::cout << "消息插入成功，ID: " << message_ops.get_last_insert_id() << std::endl;
    }
    else
    {
        std::cout << "消息插入失败" << std::endl;
    }

    // 查询消息
    std::cout << "\n7. 查询消息:" << std::endl;
    auto messages_result = message_ops.select_where("sender = 'desc_test_user'");
    for (const auto& msg_view : messages_result)
    {
        const auto& msg = msg_view.data;
        std::cout << "ID: " << msg.id << ", 发送者: " << msg.sender.content
                  << ", 内容: " << msg.content.content << ", 创建时间: " << msg.created_at.content
                  << std::endl;
    }

    // 更新用户密码 - 使用表描述信息
    std::cout << "\n8. 使用表描述信息更新用户密码:" << std::endl;
    if (!users_result.empty())
    {
        auto updated_user = users_result[0].data;
        updated_user.password = "new_desc_password";

        if (user_ops.update(updated_user, "id = " + std::to_string(updated_user.id)))
        {
            std::cout << "用户密码更新成功" << std::endl;

            // 验证更新结果
            auto updated_result = user_ops.select_where("id = " + std::to_string(updated_user.id));
            if (!updated_result.empty())
            {
                std::cout << "验证更新结果 - 新密码: " << updated_result[0].data.password.content
                          << std::endl;
            }
        }
        else
        {
            std::cout << "用户密码更新失败" << std::endl;
        }
    }

    // 清理测试数据
    std::cout << "\n9. 清理测试数据:" << std::endl;
    message_ops.remove("sender = 'desc_test_user'");
    user_ops.remove("username = 'desc_test_user'");
    std::cout << "测试数据清理完成" << std::endl;
}

void test_description_vs_auto_generation()
{
    std::cout << "\n=== 表描述信息 vs 自动生成对比 ===" << std::endl;

    // 定义一个新的测试结构体，但不设置描述信息
    struct test_table
    {
        int id;
        varchar<100> name;
        text description;
        timestamp created_at;
    };

    // 测试自动生成的描述信息
    std::cout << "\n1. 自动生成的表描述信息:" << std::endl;
    const auto& auto_desc = table_discribe<test_table>::get_description();
    std::cout << "表名: " << auto_desc.table_name << std::endl;
    std::cout << "主键: " << auto_desc.primerkey << std::endl;

    for (size_t i = 0; i < auto_desc.field_count(); ++i)
    {
        std::cout << "字段 " << i << ": " << auto_desc.field_names[i] << " "
                  << auto_desc.field_types[i] << " " << auto_desc.field_constraints[i] << std::endl;
    }

    // 对比手动配置的users表描述
    std::cout << "\n2. 手动配置的Users表描述信息:" << std::endl;
    const auto& manual_desc = table_discribe<users>::get_description();
    std::cout << "表名: " << manual_desc.table_name << std::endl;
    std::cout << "主键: " << manual_desc.primerkey << std::endl;

    for (size_t i = 0; i < manual_desc.field_count(); ++i)
    {
        std::cout << "字段 " << i << ": " << manual_desc.field_names[i] << " "
                  << manual_desc.field_types[i] << " " << manual_desc.field_constraints[i]
                  << std::endl;
    }

    std::cout << "\n注意：手动配置的描述信息更准确，包含了正确的约束条件" << std::endl;
}

// void test_json_sql_operations() {
//     std::cout << "=== JSON-SQL 操作示例 ===" << std::endl;

//     // 创建 SQL 操作实例
//     auto user_ops = make_sql_enabled<users>(sql, "users");
//     auto message_ops = make_sql_enabled<messages>(sql, "messages");

//     std::cout << "\n1. 检查表是否存在:" << std::endl;
//     std::cout << "Users table exists: " << user_ops.table_exists() << std::endl;
//     std::cout << "Messages table exists: " << message_ops.table_exists() << std::endl;

//     // 创建测试用户
//     users test_user;
//     test_user.id = 0; // 自增字段，插入时会被忽略
//     test_user.username = "test_user";
//     test_user.password = "password123";
//     // 不设置 created_at，让数据库使用默认值
//     // test_user.created_at 保持为空，会被跳过

//     std::cout << "\n2. 插入测试用户:" << std::endl;
//     if (user_ops.insert(test_user)) {
//         std::cout << "用户插入成功，ID: " << user_ops.get_last_insert_id() << std::endl;
//     } else {
//         std::cout << "用户插入失败" << std::endl;
//     }

//     // 查询用户
//     std::cout << "\n3. 查询用户:" << std::endl;
//     auto users_result = user_ops.select_where("username = 'test_user'");
//     for (const auto& user_view : users_result) {
//         const auto& user = user_view.data;
//         std::cout << "ID: " << user.id << ", 用户名: " << user.username.content
//                   << ", 密码: " << user.password.content
//                   << ", 创建时间: " << user.created_at.content << std::endl;
//     }

//     // 统计用户数量
//     std::cout << "\n4. 统计用户数量:" << std::endl;
//     std::cout << "总用户数: " << user_ops.count() << std::endl;
//     std::cout << "test_user 数量: " << user_ops.count("username = 'test_user'") << std::endl;

//     // 创建测试消息
//     messages test_message;
//     test_message.id = 0;
//     test_message.sender = "test_user";
//     test_message.content = "这是一条测试消息";
//     // 不设置 created_at，让数据库使用默认值

//     std::cout << "\n5. 插入测试消息:" << std::endl;
//     if (message_ops.insert(test_message)) {
//         std::cout << "消息插入成功，ID: " << message_ops.get_last_insert_id() << std::endl;
//     } else {
//         std::cout << "消息插入失败" << std::endl;
//     }

//     // 批量插入消息
//     std::vector<messages> batch_messages;
//     for (int i = 1; i <= 3; ++i) {
//         messages msg;
//         msg.id = 0;
//         msg.sender = "test_user";
//         msg.content = "批量消息 " + std::to_string(i);
//         // 不设置 created_at
//         batch_messages.push_back(msg);
//     }

//     std::cout << "\n6. 批量插入消息:" << std::endl;
//     if (message_ops.insert_batch(batch_messages)) {
//         std::cout << "批量插入成功" << std::endl;
//     } else {
//         std::cout << "批量插入失败" << std::endl;
//     }

//     // 查询最新消息
//     std::cout << "\n7. 查询最新消息:" << std::endl;
//     auto latest_messages = message_ops.select_where("sender = 'test_user' ORDER BY id DESC LIMIT
//     5"); for (const auto& msg_view : latest_messages) {
//         const auto& msg = msg_view.data;
//         std::cout << "ID: " << msg.id << ", 发送者: " << msg.sender.content
//                   << ", 内容: " << msg.content.content
//                   << ", 创建时间: " << msg.created_at.content << std::endl;
//     }

//     // 更新用户密码
//     std::cout << "\n8. 更新用户密码:" << std::endl;
//     if (!users_result.empty()) {
//         auto updated_user = users_result[0].data;
//         updated_user.password = "new_password456";

//         // 添加调试输出
//         std::cout << "更新用户ID: " << updated_user.id << std::endl;
//         std::cout << "新密码: " << updated_user.password.content << std::endl;

//         if (user_ops.update(updated_user, "id = " + std::to_string(updated_user.id))) {
//             std::cout << "用户密码更新成功" << std::endl;

//             // 验证更新结果
//             auto updated_result = user_ops.select_where("id = " +
//             std::to_string(updated_user.id)); if (!updated_result.empty()) {
//                 std::cout << "验证更新结果 - 新密码: " << updated_result[0].data.password.content
//                 << std::endl;
//             }
//         } else {
//             std::cout << "用户密码更新失败" << std::endl;
//         }
//     }

//     // 删除测试数据
//     std::cout << "\n9. 清理测试数据:" << std::endl;
//     message_ops.remove("sender = 'test_user'");
//     user_ops.remove("username = 'test_user'");
//     std::cout << "测试数据清理完成" << std::endl;
// }

void test_data_views_conversion()
{
    std::cout << "\n=== Data Views 转换示例 ===" << std::endl;

    // 创建测试数据
    users user;
    user.id = 123;
    user.username = "demo_user";
    user.password = "demo_pass";
    user.created_at = "2025-01-17 10:30:00";

    // 创建 data_views
    data_views<users> user_view(user);

    std::cout << "\n1. 生成 INSERT SQL (使用默认类名作为表名):" << std::endl;
    std::cout << user_view.to_insert_sql() << std::endl;

    std::cout << "\n2. 生成 INSERT SQL (使用自定义表名):" << std::endl;
    std::cout << user_view.to_insert_sql("custom_users_table") << std::endl;

    std::cout << "\n3. 生成 UPDATE SQL (使用默认类名作为表名):" << std::endl;
    std::cout << user_view.to_update_sql("id = 123") << std::endl;

    std::cout << "\n4. 生成 UPDATE SQL (使用自定义表名):" << std::endl;
    std::cout << user_view.to_update_sql("id = 123", "custom_users_table") << std::endl;

    // 模拟从数据库结果构造对象
    std::cout << "\n5. 从查询结果构造对象:" << std::endl;
    std::vector<std::string> mock_row = {"456", "result_user", "result_pass",
                                         "2025-01-17 11:00:00"};
    auto result_view = data_views<users>::from_result_row(mock_row);

    if (result_view)
    {
        const auto& result_user = result_view->data;
        std::cout << "从结果集构造的用户: ID=" << result_user.id
                  << ", 用户名=" << result_user.username.content
                  << ", 密码=" << result_user.password.content << std::endl;
    }
}

void test_sql_types()
{
    std::cout << "\n=== SQL 类型测试 ===" << std::endl;

    // 测试不同的 SQL 类型
    varchar<100> name = "张三";
    text description;
    description = "这是一个详细的描述文本";
    bigint large_number = 1234567890LL; // 直接使用long long字面量
    timestamp time_value = "2025-01-17 12:00:00";

    std::cout << "VARCHAR(100): " << name.content << std::endl;
    std::cout << "TEXT: " << description.content << std::endl;
    std::cout << "BIGINT: " << large_number.content << " (转换为数值: " << (long long)large_number
              << ")" << std::endl;
    std::cout << "TIMESTAMP: " << time_value.content << std::endl;
}

int main()
{
    std::cout << "启动 JSON-SQL 演示程序（带表描述信息）..." << std::endl;

    // 检查数据库连接
    if (!sql.getConnection())
    {
        std::cerr << "数据库连接失败，程序退出" << std::endl;
        return 1;
    }

    std::cout << "数据库连接成功！" << std::endl;

    try
    {
        // 运行各种测试
        test_sql_types();
        test_table_description();
        test_description_vs_auto_generation();
        test_data_views_conversion();
        test_json_sql_operations();

        std::cout << "\n=== 所有测试完成 ===" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}