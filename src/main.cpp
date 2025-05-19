#include "main.hpp"
#include "format.h"
#include "httpServerFile.h"     // 确保包含HttpServerUtil的头文件 and Protocol::HTTP
#include "protocol_constants.h" // 新增包含
#include "serverFile.h"
#include <chrono>
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <iostream>
#include <mysqlHandle.h> // 添加MySQL处理类的头文件
#include <mysqlHandle.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <random> // 添加随机数生成支持
#include <string>
#include <webSocketFile.h> // For Protocol::WebSocket

// 生成随机会话ID
std::string generateRandomSessionId(size_t length = 32)
{
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(length);

    // 使用随机设备作为种子
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

    for (size_t i = 0; i < length; ++i)
    {
        result += alphanum[dis(gen)];
    }

    return result;
}

// 格式化时间
std::string formatTime(std::time_t time)
{
    char buffer[30];
    std::tm* timeinfo = std::localtime(&time);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

struct Message
{
    std::string sender = "";
    std::string content = "";
    // 2025-05-17 14:23:55
    std::string time = "1970-01-01 00:00::00";
    Message(std::string asformat)
    {
        // 解析格式化字符串，例如 "sender:张三\ncontent:你好\ntime:2023-06-01 12:30:45"
        size_t pos = 0;
        size_t end = 0;

        // 解析发送者
        if ((pos = asformat.find("sender:")) != std::string::npos)
        {
            pos += 7; // "sender:"的长度
            end = asformat.find('\n', pos);
            if (end != std::string::npos)
            {
                sender = asformat.substr(pos, end - pos);
            }
        }

        // 解析内容
        if ((pos = asformat.find("content:")) != std::string::npos)
        {
            pos += 8; // "content:"的长度
            end = asformat.find('\n', pos);
            if (end != std::string::npos)
            {
                content = asformat.substr(pos, end - pos);
            }
        }

        // 解析时间
        if ((pos = asformat.find("time:")) != std::string::npos)
        {
            pos += 5; // "time:"的长度
            end = asformat.find('\n', pos);
            if (end != std::string::npos)
            {
                time = asformat.substr(pos, end - pos);
            }
            else
            {
                time = asformat.substr(pos); // 如果没有换行符就取到末尾
            }
        }
    }

    Message(std::vector<std::string> asformat)
    {
        // 根据数据库实际结构调整索引
        // 数据库返回的行按照id, sender, content, timestamp, created_at的顺序
        if (asformat.size() >= 5)
        {
            sender = asformat[1];  // 发送者在索引1
            content = asformat[2]; // 内容在索引2
            // 使用created_at作为时间
            time = asformat[4]; // created_at在索引4
        }
    }

    operator std::string()
    {
        return "sender: " + sender + '\n' + "content: " + content + '\n' + "time: " + time + '\n' +
               '\n';
    }
};
// std::map<int, serverFile*> wsconnecting;
auto httpServer = HttpServer{};

// 修改全局变量初始化方式，避免在程序启动前执行MySQL连接
MySQLHandle mysqldb;
std::map<std::string, std::string> tekon_name;
int main()
{
    LocalFiles static_files;
    control con;
    auto& coManagerInstance = Co_Start_Manager::getInstance(); // Renamed for clarity

    // 在程序启动后连接数据库
    if (!mysqldb.connect("localhost", "webserver", "WebServer@2025", "chat_db", 3306))
    {
        std::cerr << "数据库连接失败: " << mysqldb.getLastError() << std::endl;
    }
    else
    {
        std::cout << "数据库连接成功!" << std::endl;
    }

    httpServer.addCallbackFormat(
        Format{"/", Format::Type::same},
        [](serverFile& socketfile)
        {
            if (socketfile.getAgreementType() == Protocol::HTTP) // 使用 Protocol 枚举
            {
                socketfile.write(HttpResponse::formLocalFile("index.html"));
            }
            return;
        });
    httpServer.addCallbackFormat(
        Format{"/getHistory", Format::Type::same},
        [](serverFile& socketfile)
        {
            if (socketfile.getAgreementType() == Protocol::HTTP) // 使用 Protocol 枚举
            {
                // 修改查询以使用created_at列而不是time列
                auto res =
                    mysqldb.query("SELECT * FROM messages ORDER BY created_at DESC LIMIT 100");
                if (!res)
                {
                    std::cerr << "查询历史消息失败: " << mysqldb.getLastError() << '\n';
                    socketfile.write(HttpResponse{500}.with_content("数据库查询失败"));
                    return;
                }
                std::string response = "";
                std::vector<std::string> row;
                while (!(row = mysqldb.fetchRow()).empty())
                {
                    response += Message{row};
                }
                auto httpres = HttpResponse{200}.with_content(response);
                socketfile.write(httpres);
            }
            return;
        });
    httpServer.addCallbackFormat(
        Format{"/wsconnect/%s", Format::Type::scanf},
        [](serverFile& socketfile)
        {
            if (socketfile.getAgreementType() == Protocol::HTTP) // 使用 Protocol 枚举
            {
                if (WebSocketUtil::shouldbeUpdataToWS(socketfile))
                {
                    socketfile.write(WebSocketUtil::makeWebSocketHandshake(socketfile));
                    socketfile.upgradeProtocol(Protocol::WebSocket);
                    return;
                }
                else
                {
                    socketfile.write(HttpResponse{404});
                    return;
                }
            }
            else if (socketfile.getAgreementType() == Protocol::WebSocket)
            {
                // 处理接收到的WebSocket消息
                auto messageStr = socketfile.getContent()["message"];
                if (messageStr.empty())
                {
                    return;
                }

                // 解析消息
                Message pullMessage{messageStr};

                // 设置当前时间（如果消息中没有提供）
                if (pullMessage.time == "1970-01-01 00:00::00")
                {
                    auto now = std::chrono::system_clock::now();
                    auto tt = std::chrono::system_clock::to_time_t(now);
                    std::tm local_tm = *std::localtime(&tt);

                    char buffer[30];
                    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_tm);
                    pullMessage.time = buffer;
                }

                // 存储消息到数据库，使用timestamp和created_at字段
                std::string escSender = pullMessage.sender;
                std::string escContent = pullMessage.content;

                // 获取当前Unix时间戳
                auto now = std::chrono::system_clock::now();
                auto unix_timestamp =
                    std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
                        .count();

                // 修改插入语句以适应表结构
                std::string insertSQL =
                    "INSERT INTO messages (sender, content, timestamp) VALUES ('" + escSender +
                    "', '" + escContent + "', " + std::to_string(unix_timestamp) + ")";

                if (!mysqldb.query(insertSQL))
                {
                    std::cerr << "存储消息失败: " << mysqldb.getLastError() << '\n';
                }

                // 广播消息给所有其他WebSocket连接
                for (auto& each : httpServer.getfilemap())
                {
                    auto& file = each.second;
                    if (file && file->getAgreementType() == Protocol::WebSocket)
                    {
                        // 不给发送者自己发消息
                        if (socketfile.getSocketFile().getfd() != file->getSocketFile().getfd())
                        {
                            file->write(std::string(WebSocketResponse::text(messageStr)));
                        }
                    }
                }
            }
            return;
        });
    httpServer.addCallbackFormat(
        Format{"/checkSession", Format::Type::same},
        [](serverFile& socketfile)
        {
            // 从请求中获取Cookie
            std::string cookieHeader = socketfile.getContent()["Cookie"];
            std::string sessionId = "";

            // 解析Cookie中的sessionId
            size_t pos = cookieHeader.find("sessionId=");
            if (pos != std::string::npos)
            {
                pos += 10; // "sessionId="的长度
                size_t end = cookieHeader.find(';', pos);
                if (end != std::string::npos)
                {
                    sessionId = cookieHeader.substr(pos, end - pos);
                }
                else
                {
                    sessionId = cookieHeader.substr(pos);
                }
            }

            // 验证会话
            if (!sessionId.empty() && tekon_name.find(sessionId) != tekon_name.end())
            {
                // 从会话中提取用户名
                std::string sessionInfo = tekon_name[sessionId];
                std::string username = "";

                size_t usernamePos = sessionInfo.find("username:");
                if (usernamePos != std::string::npos)
                {
                    usernamePos += 9; // "username:"的长度
                    size_t usernameEnd = sessionInfo.find('\n', usernamePos);
                    if (usernameEnd != std::string::npos)
                    {
                        username = sessionInfo.substr(usernamePos, usernameEnd - usernamePos);
                    }
                }

                // 返回成功响应
                HttpResponse response{200};
                response.with_content("{\"status\":\"success\",\"username\":\"" + username + "\"}");
                socketfile.write(response);
            }
            else
            {
                // 会话无效
                HttpResponse response{401};
                response.with_content("{\"status\":\"error\",\"message\":\"未登录或会话已过期\"}");
                socketfile.write(response);
            }
        });
    httpServer.addCallbackFormat(
        Format{"/login/%s/%s", Format::Type::scanf},
        [](serverFile& socketfile)
        {
            std::optional<Format::ParseResult> res =
                Format{"/login/%s/%s", Format::Type::scanf}.parse(socketfile.getContent()["path"]);
            if (res)
            {
                auto username = std::get<std::string>((*res)[0]);
                auto password = std::get<std::string>((*res)[1]);

                // 修正SQL查询语句
                auto query_res = mysqldb.query("SELECT * FROM users WHERE username = '" + username +
                                               "' AND password = '" + password + "'");
                if (query_res)
                {
                    // 检查是否有结果
                    auto row = mysqldb.fetchRow();
                    if (!row.empty())
                    {
                        // 生成随机会话ID
                        std::string sessionId = generateRandomSessionId();

                        // 获取当前时间
                        auto now = std::chrono::system_clock::now();
                        auto now_time_t = std::chrono::system_clock::to_time_t(now);

                        // 设置过期时间（例如，1小时后）
                        auto expiry = now + std::chrono::hours(1);
                        auto expiry_time_t = std::chrono::system_clock::to_time_t(expiry);

                        // 格式化时间
                        std::string currentTime = formatTime(now_time_t);
                        std::string expiryTime = formatTime(expiry_time_t);

                        // 在服务器端存储会话信息
                        tekon_name[sessionId] = "username:" + username + '\n' +
                                                "loginTime:" + currentTime + '\n' +
                                                "expiryTime:" + expiryTime + '\n';

                        // 返回带有Set-Cookie头的响应
                        HttpResponse response{200};
                        response.addHeader("Set-Cookie", "sessionId=" + sessionId +
                                                             "; Path=/; HttpOnly; Max-Age=3600");
                        response.with_content(
                            "{\"status\":\"success\",\"message\":\"登录成功\",\"username\":\"" +
                            username + "\"}");
                        socketfile.write(response);
                    }
                    else
                    {
                        // 无匹配用户
                        HttpResponse response{401};
                        response.with_content(
                            "{\"status\":\"error\",\"message\":\"用户名或密码错误\"}");
                        socketfile.write(response);
                    }
                }
                else
                {
                    HttpResponse response{500};
                    response.with_content("{\"status\":\"error\",\"message\":\"数据库查询出错\"}");
                    socketfile.write(response);
                }
            }
            else
            {
                socketfile.write(HttpResponse{400}.with_content(
                    "{\"status\":\"error\",\"message\":\"请求格式错误\"}"));
            }
        });

    // 确保数据库中有users表，如果没有则创建
    if (!mysqldb.query("CREATE TABLE IF NOT EXISTS users ("
                       "id INT AUTO_INCREMENT PRIMARY KEY, "
                       "username VARCHAR(50) NOT NULL UNIQUE, "
                       "password VARCHAR(100) NOT NULL, "
                       "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"))
    {
        std::cerr << "创建users表失败: " << mysqldb.getLastError() << '\n';
    }
    else
    {
        // 检查是否有默认用户，如果没有则添加一个
        auto res = mysqldb.query("SELECT COUNT(*) FROM users");
        if (res)
        {
            auto row = mysqldb.fetchRow();
            if (!row.empty() && std::stoi(row[0]) == 0)
            {
                // 添加默认管理员用户
                if (!mysqldb.query(
                        "INSERT INTO users (username, password) VALUES ('admin', 'admin123')"))
                {
                    std::cerr << "添加默认用户失败: " << mysqldb.getLastError() << '\n';
                }
                else
                {
                    std::cout << "已添加默认用户：admin/admin123\n";
                }
            }
        }
    }

    // 确保数据库中有messages表，如果没有则创建
    if (!mysqldb.query("CREATE TABLE IF NOT EXISTS messages ("
                       "id INT AUTO_INCREMENT PRIMARY KEY, "
                       "sender VARCHAR(50) NOT NULL, "
                       "content TEXT NOT NULL, "
                       "timestamp BIGINT NOT NULL, "
                       "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"))
    {
        std::cerr << "创建messages表失败: " << mysqldb.getLastError() << '\n';
    }
    else
    {
        std::cout << "数据库和消息表初始化成功\n";
    }

    // 执行MySQL示例
    // mysqlExample();

    coManagerInstance.getManager().add(httpServer);                 // Use getter for manager
    coManagerInstance.getManager().add(con);                        // Use getter for manager
    coManagerInstance.setLoopInterval(std::chrono::nanoseconds(0)); // Use setLoopInterval

    // 启动服务器
    coManagerInstance.start();

    return 0;
}
