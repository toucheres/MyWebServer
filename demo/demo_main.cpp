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
#include <array>
#include <iostream>
#include <string_view>
#include <utility>

struct book
{
    std::string bookname;
    int price;
};

struct person
{
    int age;
    std::string name;
    book a;
};

int main()
{
    static constexpr auto pt = make_fake_constexpr_memberptr_tuple_form_type<person>();
    // num_of_number_with_inner
    constexpr std::array<std::pair<std::string_view, std::string_view>, 7> arr = {};
    auto a = num_of_number_with_inner_v<decltype(arr)>;
    // for (auto& m : a)
    // {
    //     std::cout << m << "\n";
    // }
    std::cout << a;
    return 0;
}
