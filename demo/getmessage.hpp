#pragma once
#include "json_sql.hpp"
#include "message.h"
#include "mysqlHandle.h"
#include <cstring>
#include <format.h>
#include <string>
#include <vector>
inline auto getmessage()
{
    static auto sqlconnection = MySQLHandle("localhost", "webserver", "WebServer@2025", "chat_db");
    auto message_connecion = enable_sql<messages>(sqlconnection);
    auto ret = message_connecion.select_all();
    return ret;
}
inline auto insertmessage(messages in)
{
    static auto sqlconnection = MySQLHandle("localhost", "webserver", "WebServer@2025", "chat_db");
    auto message_connecion = enable_sql<messages>(sqlconnection);
    auto ret = message_connecion.insert(in);
    return ret;
}
