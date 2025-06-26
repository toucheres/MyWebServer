#pragma once
#include "StringCrypto.h"
#include "json.hpp"
#include "json_sql.hpp"
#include "mysqlHandle.h"
#include "users.h"
#include <chrono>
#include <cstring>
#include <format>
#include <openssl/hmac.h>
#include <optional>
#include <string>
#include <vector>
// std::string getDailyKeyHMAC()
// {
//     // 获取当前日期
//     auto now = std::chrono::system_clock::now();
//     auto time_t = std::chrono::system_clock::to_time_t(now);
//     auto tm = *std::localtime(&time_t);

//     std::stringstream ss;
//     ss << std::put_time(&tm, "%Y-%m-%d");
//     std::string date_str = ss.str();

//     // 使用HMAC-SHA256
//     std::string secret_key = "YourMasterSecretKey2025";
//     unsigned char* digest = HMAC(EVP_sha256(), secret_key.c_str(), secret_key.length(),
//                                  reinterpret_cast<const unsigned char*>(date_str.c_str()),
//                                  date_str.length(), nullptr, nullptr);

//     // 转换为十六进制
//     std::stringstream hex_ss;
//     for (int i = 0; i < 32; i++)
//     {
//         hex_ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
//     }

//     return hex_ss.str();
// }
inline std::string key = [](){    // 获取当前日期
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);

    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d");
    std::string date_str = ss.str();

    // 使用HMAC-SHA256
    std::string secret_key = "hjh060823";
    unsigned char* digest = HMAC(EVP_sha256(), secret_key.c_str(), secret_key.length(),
                                 reinterpret_cast<const unsigned char*>(date_str.c_str()),
                                 date_str.length(), nullptr, nullptr);

    // 转换为十六进制
    std::stringstream hex_ss;
    for (int i = 0; i < 32; i++)
    {
        hex_ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    return hex_ss.str();
}();

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

inline std::optional<users> check(const std::string& cookie)
{
    auto decode = StringCrypto::decryptFromBase64(cookie, key);
    try
    {
        auto ret = json::to<users>(decode);
        return ret;
    }
    catch (...)
    {
        return std::nullopt;
    }
    return std::nullopt;
}

inline auto register_user(const std::string& username, const std::string& password)
{
    static auto sqlconnection = MySQLHandle("localhost", "webserver", "WebServer@2025", "chat_db");
    auto users_connecion = enable_sql<users>(sqlconnection);
    auto ret = users_connecion.select_where(std::format("username = '{}'", username));
    if (ret.size())
    {
        return false;
    }
    users tp;
    tp.username.content = username;
    tp.password.content = password;
    return users_connecion.insert(tp);
}
inline auto deregistration(const std::string& username, const std::string& password)
{
    static auto sqlconnection = MySQLHandle("localhost", "webserver", "WebServer@2025", "chat_db");
    auto users_connecion = enable_sql<users>(sqlconnection);
    auto ret = users_connecion.select_where(std::format("username = '{}'", username));
    if (ret.size())
    {
        users_connecion.remove(std::format("username = '{}'", username));
        return true;
    }
    return false;
}