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
// | timestamp  | bigint      | NO   |     | NULL              |                   |
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
#include "json_sql.hpp"
#include "message.h"
#include "users.h"
#include <iostream>

int main()
{
    // // 创建一个 users 对象
    // users user;
    // user.id = 0; // 会被跳过，因为通常是自增的
    // user.username = "newuser";
    // user.password = "userpassword";

    // // 生成 INSERT SQL 语句
    // std::string insert_sql = JsonSql::toInsertSql(user, "users");
    // std::cout << "INSERT SQL: " << insert_sql << std::endl;

    // // 从 SQL 语句解析用户数据
    // std::string sql_with_values =
    //     "INSERT INTO users (username, password) VALUES ('parsed_user', 'parsed_password')";

    // // 修改：使用命名捕获组的正则表达式，确保字段与值的正确匹配
    // users parsed_user = JsonSql::fromInsertSql<users>(
    //     sql_with_values, "INSERT INTO users \\(([^,]+),\\s*([^)]+)\\) "
    //                      "VALUES\\s*\\(['\"](.*?)['\"](\\s*),\\s*['\"](.*?)['\"]\\)");

    // // 显示解析结果
    // std::cout << "Parsed username: " << parsed_user.username << std::endl;
    // std::cout << "Parsed password: " << parsed_user.password << std::endl;

    // // 生成 UPDATE SQL 语句
    // std::string update_sql = JsonSql::toUpdateSql(user, "users", "id = 1");
    // std::cout << "UPDATE SQL: " << update_sql << std::endl;

    // // 从 UPDATE SQL 语句解析
    // std::string update_sql_example =
    //     "UPDATE users SET username = 'updated_user', password = 'updated_pass' WHERE id = 1";
    // users updated_user = JsonSql::fromUpdateSql<users>(update_sql_example);

    // // 显示解析结果
    // std::cout << "Updated username: " << updated_user.username << std::endl;
    // std::cout << "Updated password: " << updated_user.password << std::endl;

    // fun<users>();
    return 0;
}
