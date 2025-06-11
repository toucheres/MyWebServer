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

#include "corutine.hpp"
#include "file.h"
#include "http.h"
#include "mysqlHandle.h"
#include "signal_slots.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
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
int main()
{
    auto ret = sql.query("select*from users");
    for (int i = 0; i < ret->getRowCount(); i++)
    {
        auto row = ret->fetchRow();
        std::cout << "id: " << row[0] << " name: " << row[1] << " password: " << row[2]
                  << " created_at: " << row[3] << '\n';
    }
}