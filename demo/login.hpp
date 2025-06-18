#pragma once
#include "StringCrypto.h"
#include "format.h"
#include "json.hpp"
#include "json_sql.hpp"
#include "mysqlHandle.h"
#include "users.h"
#include <array>
#include <cstring>
#include <format>
#include <iostream>
#include <span>
#include <string>
#include <vector>

inline std::string key = "123";

inline auto login(const std::string& username, const std::string& password)
{
    static auto sqlconnection = MySQLHandle("localhost", "webserver", "WebServer@2025", "chat_db");
    auto users_connecion = enable_sql<users>(sqlconnection);
    auto ret = users_connecion.select_where(
        std::format("username = '{}' and password = '{}'", username, password));
    if (ret.size())
    {
        users tp;
        tp.username.content = username;
        tp.password.content = password;
        auto imf = (std::string)json::from(tp);
        return StringCrypto::encryptToBase64(imf, key);
    }
    else
    {
        return std::string{""};
    }
}

inline auto check(const std::string& cookie)
{
    auto decode = StringCrypto::decryptFromBase64(cookie, key);
    try
    {
        auto ret = json::to<users>(decode);
        return ret;
    }
    catch (...)
    {
        return users{};
    }
    return users{};
}
