#include "demo_routes.h"
#include "demo_util.h"
#include "demo_message.h"
#include "httpServerFile.h"
#include "webSocketFile.h"
#include "protocol_constants.h"
#include <chrono>
#include <iostream>
#include <mysqlHandle.h>

// 定义外部声明的全局变量
std::map<std::string, std::string> token_name;
extern MySQLHandle mysqldb;

void registerAllRoutes(HttpServer& httpServer)
{
    // 根路径处理
    httpServer.addCallbackFormat(Format{"/", Format::Type::same}, handleRoot);
    
    // 获取历史消息
    httpServer.addCallbackFormat(Format{"/getHistory", Format::Type::same}, handleGetHistory);
    
    // WebSocket连接
    httpServer.addCallbackFormat(Format{"/wsconnect/%s", Format::Type::scanf}, handleWebSocketConnect);
    
    // 会话检查
    httpServer.addCallbackFormat(Format{"/checkSession", Format::Type::same}, handleCheckSession);
    
    // 用户登录
    httpServer.addCallbackFormat(Format{"/login/%[^/]/%s", Format::Type::scanf}, handleLogin);
    
    // 用户注册
    httpServer.addCallbackFormat(Format{"/register/%[^/]/%s", Format::Type::scanf}, handleRegister);
}

void handleRoot(serverFile& socketfile)
{
    if (socketfile.getAgreementType() == Protocol::HTTP)
    {
        socketfile.write(HttpResponse::formLocalFile("index.html"));
    }
}

void handleGetHistory(serverFile& socketfile)
{
    if (socketfile.getAgreementType() == Protocol::HTTP)
    {
        // 查询历史消息
        auto res = mysqldb.query("SELECT * FROM messages ORDER BY created_at DESC LIMIT 100");
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
}

void handleWebSocketConnect(serverFile& socketfile)
{
    if (socketfile.getAgreementType() == Protocol::HTTP)
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

        // 存储消息到数据库
        std::string escSender = pullMessage.sender;
        std::string escContent = pullMessage.content;

        // 获取当前Unix时间戳
        auto now = std::chrono::system_clock::now();
        auto unix_timestamp =
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
                .count();

        // 插入消息
        std::string insertSQL =
            "INSERT INTO messages (sender, content, timestamp) VALUES ('" + escSender +
            "', '" + escContent + "', " + std::to_string(unix_timestamp) + ")";

        if (!mysqldb.query(insertSQL))
        {
            std::cerr << "存储消息失败: " << mysqldb.getLastError() << '\n';
        }

        // 广播消息给所有其他WebSocket连接
        // 修复：使用全局httpServer变量而非不存在的getServer()方法
        extern HttpServer httpServer;
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
}

void handleCheckSession(serverFile& socketfile)
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
    if (!sessionId.empty() && token_name.find(sessionId) != token_name.end())
    {
        // 从会话中提取用户名
        std::string sessionInfo = token_name[sessionId];
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
}

void handleLogin(serverFile& socketfile)
{
    std::optional<Format::ParseResult> res =
        Format{"/login/%[^/]/%s", Format::Type::scanf}.parse(
            socketfile.getContent()["path"]);
    
    if (res)
    {
        auto username = std::get<std::string>((*res)[0]);
        auto password = std::get<std::string>((*res)[1]);

        // 查询用户
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
                token_name[sessionId] = "username:" + username + '\n' +
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
}

void handleRegister(serverFile& socketfile)
{
    std::optional<Format::ParseResult> res =
        Format{"/register/%[^/]/%s", Format::Type::scanf}.parse(
            socketfile.getContent()["path"]);
    
    if (res)
    {
        auto username = std::get<std::string>((*res)[0]);
        auto password = std::get<std::string>((*res)[1]);

        // 验证用户名和密码
        if (username.length() < 3)
        {
            HttpResponse response{400};
            response.with_content("{\"status\":\"error\",\"message\":\"用户名至少需要3个字符\"}");
            socketfile.write(response);
            return;
        }

        if (password.length() < 6)
        {
            HttpResponse response{400};
            response.with_content("{\"status\":\"error\",\"message\":\"密码至少需要6个字符\"}");
            socketfile.write(response);
            return;
        }

        // 检查用户名是否已存在
        auto query_res = mysqldb.query("SELECT * FROM users WHERE username = '" + username + "'");
        if (!query_res)
        {
            HttpResponse response{500};
            response.with_content("{\"status\":\"error\",\"message\":\"数据库查询出错\"}");
            socketfile.write(response);
            return;
        }

        if (!mysqldb.fetchRow().empty())
        {
            // 用户名已存在
            HttpResponse response{409}; // Conflict
            response.with_content("{\"status\":\"error\",\"message\":\"用户名已存在\"}");
            socketfile.write(response);
            return;
        }

        // 插入新用户
        if (!mysqldb.query(
                "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "')"))
        {
            HttpResponse response{500};
            response.with_content("{\"status\":\"error\",\"message\":\"创建用户失败\"}");
            socketfile.write(response);
            return;
        }

        // 注册成功
        HttpResponse response{200};
        response.with_content("{\"status\":\"success\",\"message\":\"注册成功\"}");
        socketfile.write(response);
    }
    else
    {
        socketfile.write(HttpResponse{400}.with_content(
            "{\"status\":\"error\",\"message\":\"请求格式错误\"}"));
    }
}
