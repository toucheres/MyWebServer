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
#include "signal_slots.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
class stdiolistener : public co_async, enable_auto_remove_slots
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

int testfun(int, double)
{
    std::cout << "hello\n";
    return 0;
}
class watcher : public enable_auto_remove_slots
{
  public:
    int state = 3;
    void fun()
    {
        std::cout << "state: " << state << '\n';
    }
    ~watcher()
    {
        std::cout << "watcher distruct\n";
    }
};

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
    // []<std::size_t... I>(std::index_sequence<I...>) { testfun(get_args_type<testfun, I>{}...); }(
    //     std::make_index_sequence<get_args_count_v<testfun>>{});
    stdiolistener a;
    auto ptr_watch = new watcher{};
    a.on_user_input.connect(*ptr_watch, [ptr_watch]() { ptr_watch->fun(); });
    a.on_user_input.connect(
        [ptr_watch](std::string_view in)
        {
            if (in == "del\n")
            {
                delete ptr_watch;
                std::cout << "ptr_watch deleted\n";
            };
        });
    auto& corus = Co_Start_Manager::getInstance();
    corus.getManager().add(a);
    corus.start();
}