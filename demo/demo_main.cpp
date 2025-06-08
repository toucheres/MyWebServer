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
#include "reflection.hpp"
#include "signal_slots.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
class stdiolistener : public co_async
{
  public:
    signal<std::string_view> on_user_input;
    signal<> on_exit;
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

int testfun(int a, int b)
{
}

int main()
{
    // stdiolistener a;
    // a.on_user_input.connect([](std::string_view in)
    //                         { std::cout << "get something: " << in << '\n'; });
    // a.on_exit.connect([]() { exit(0); });
    // auto& corus = Co_Start_Manager::getInstance();
    // corus.getManager().add(a);
    // corus.start();
    // std::cout << b;
    constexpr int a =
        typeVector<int, long, long long>::insert<double, 1>::divid<1>::secend::find<double>;
    std::cout << a << '\n';
}