#include "corutine.hpp"
#include "debug.hpp"
#include "file.h"
#include "format.h"
#include "forworder.h"
#include "getmessage.hpp"
#include "http.h"
#include "httpClient.h"
#include "httpServerFile.h"
#include "json.hpp"
#include "login.hpp"
#include "message.h"
#include "platform.h"
#include "serverFile.h"
#include "stdiomanager.hpp"
#include "threadpool.hpp"
#include "users.h"
#include "webSocketFile.h"
#include <chrono>
#include <cstdlib>
#include <string>
#include <thread>
int each_process(uint16_t port)
{

    // printf("线程生效\n");
    LocalFiles LocalFileCache;
    HttpServer server{port};
    // std::cout << std::format("http://localhost:{}", server.getPort()) << '\n';
    json::defultOpt.flatten_single_member = true;

    server.autoLoginFile(LocalFileCache);
    server.addCallbackFormat(
        Format{"/login/%[^/]/%s", Format::Type::scanf},
        [&server](serverFile& connection)
        {
            auto ret = Format{"/login/%[^/]/%s", Format::Type::scanf}.parse(
                connection.getContent()[HttpResponse::RequestKey::path]);
            if (ret)
            {
                auto context = *ret;
                auto username = std::get<std::string>(context[0]);
                auto password = std::get<std::string>(context[1]);
                auto logined = login(username, password);
                if (logined.size())
                {
                    std::string cookie_value{logined};
                    auto httpres =
                        HttpResponse{200}
                            .with_content("Login successful")
                            .addHeader(HttpResponse::ResponseKey::Set_Cookie,
                                       "session=" + cookie_value +
                                           "; Path=/; HttpOnly; SameSite=Lax")
                            .addHeader(HttpResponse::ResponseKey::Access_Control_Allow_Credentials,
                                       "true")
                            .addHeader(HttpResponse::ResponseKey::Access_Control_Allow_Origin,
                                       std::format("http://127.0.0.1:{}", server.getPort()));
                    // std::cout << "login reply: " << (std::string)httpres << '\n';
                    connection << httpres;
                }
                else
                {
                    connection << HttpResponse{403};
                }
            }
            else
            {
                connection << HttpResponse{400};
            }
        });
    server.addCallbackFormat(
        Format{"/ws", Format::Type::same},
        [&server](serverFile& file)
        {
            // std::cout << "ok" << '\n';
            // auto cookie = file.getContent()["cookie"];
            auto cookie = file.getContent()[HttpResponse::RequestKey::cookie];
            // std::cout << "Received cookie: " << cookie << std::endl;

            // 从cookie字符串中提取session值
            std::string session_cookie;
            size_t session_pos = cookie.find("session=");
            if (session_pos != std::string::npos)
            {
                size_t start = session_pos + 8; // "session="的长度
                size_t end = cookie.find(";", start);
                if (end == std::string::npos)
                {
                    end = cookie.length();
                }
                session_cookie = cookie.substr(start, end - start);
            }

            auto ret = check(session_cookie);
            if (!ret)
            {
                file << HttpResponse{403};
            }
            else
            {
                if (file.getAgreementType() == Protocol::WebSocket)
                {
                    auto mes = file.getContent()[WebSocketUtil::ContentKey::message];
                    auto ret =
                        Format("username = %[^,], content = %s", Format::Type::scanf).parse(mes);
                    if (ret)
                    {
                        auto name = std::get<std::string>((*ret)[0]);
                        auto con = std::get<std::string>((*ret)[1]);
                        messages mess;
                        mess.sender = name;
                        mess.content = con;
                        insertmessage(mess);
                        for (auto& each : server.getfilemap())
                        {
                            if (each.second->getContent()["inws"] == "ok")
                            {
                                (*each.second) << WebSocketResponse::text(mes);
                            }
                        }
                    }
                }
                else if (WebSocketUtil::tryUpToWs(file))
                {
                    file.getContent()["inws"] = "ok";
                }
                else
                {
                    file << HttpResponse{404};
                }
            }
        });
    server.addCallbackFormat(Format{"/do", Format::Type::same},
                             [](serverFile& file)
                             {
                                 // std::cout << "ok" << '\n';
                                 auto cookie = file.getContent()[HttpResponse::RequestKey::cookie];
                                 // std::cout << "Received cookie in /do: " << cookie << std::endl;

                                 // 从cookie字符串中提取session值
                                 std::string session_cookie;
                                 size_t session_pos = cookie.find("session=");
                                 if (session_pos != std::string::npos)
                                 {
                                     size_t start = session_pos + 8; // "session="的长度
                                     size_t end = cookie.find(";", start);
                                     if (end == std::string::npos)
                                     {
                                         end = cookie.length();
                                     }
                                     session_cookie = cookie.substr(start, end - start);
                                 }

                                 auto ret = check(session_cookie);
                                 if (!ret)
                                 {
                                     file << HttpResponse{403};
                                 }
                                 else
                                 {
                                     file << HttpResponse::text(
                                         std::format("your name is: {}, your password is {}",
                                                     ret->username.content, ret->password.content));
                                 }
                             });

    server.addCallbackFormat(Format{"/getmessages", Format::Type::same},
                             [](serverFile& file)
                             {
                                 // std::cout << "ok" << '\n';
                                 auto cookie = file.getContent()[HttpResponse::RequestKey::cookie];
                                 // std::cout << "Received cookie: " << cookie << std::endl;

                                 // 从cookie字符串中提取session值
                                 std::string session_cookie;
                                 // std::cout << "cookie before: " << cookie << '\n';
                                 size_t session_pos = cookie.find("session=");
                                 if (session_pos != std::string::npos)
                                 {
                                     size_t start = session_pos + 8; // "session="的长度
                                     size_t end = cookie.find(";", start);
                                     if (end == std::string::npos)
                                     {
                                         end = cookie.length();
                                     }
                                     session_cookie = cookie.substr(start, end - start);
                                 }

                                 // std::cout << "Extracted session: " << session_cookie <<
                                 // std::endl;

                                 auto ret = check(session_cookie);
                                 if (!ret)
                                 {
                                     //  std::cout << "cookie验证失败\n";
                                     //  std::cout << "origin imf: \n"
                                     //            << file.getSocketFile().read_all() << '\n';
                                     //  std::cout << "parsed imf:\n";
                                     //  for (auto& each : file.getContent())
                                     //  {
                                     //      std::cout << each.first << ": " << each.second << '\n';
                                     //  }
                                     //  std::cout << '\n';
                                     // std::cout << session_cookie << '\n';
                                     file << HttpResponse{403};
                                 }
                                 else
                                 {
                                     //  std::cout << "cookie验证成功\n";
                                     //  std::cout << session_cookie << '\n';
                                     auto ret = getmessage();
                                     std::string res = "[";
                                     bool first = true;
                                     for (auto& each : ret)
                                     {
                                         if (!first)
                                         {
                                             res += ",";
                                         }
                                         res += json::from(each.to_struct());
                                         first = false;
                                     }
                                     res += "]";
                                     file << HttpResponse::text(res);
                                 }
                             });

    server.addCallbackFormat(Format{"/api%s", Format::Type::scanf},
                             [&server](serverFile& file)
                             {
                                 auto cookie = file.getContent()[HttpResponse::RequestKey::cookie];
                                 // std::cout << "Received cookie in /do: " << cookie << std::endl;
                                 // std::cout <<
                                 // file.getContent()[HttpResponse::RequestKey::orignal_content] <<
                                 // '\n'; 从cookie字符串中提取session值
                                 std::string session_cookie;
                                 size_t session_pos = cookie.find("session=");
                                 if (session_pos != std::string::npos)
                                 {
                                     size_t start = session_pos + 8; // "session="的长度
                                     size_t end = cookie.find(";", start);
                                     if (end == std::string::npos)
                                     {
                                         end = cookie.length();
                                     }
                                     session_cookie = cookie.substr(start, end - start);
                                 }
                                 // auto cookie_ =
                                 // file.getContent()[HttpResponse::ContentKey::cookie];
                                 auto ret = check(session_cookie);
                                 if (!ret)
                                 {
                                     // file.write(HttpServerUtil::makeHttp(403, ""));
                                     file << HttpResponse{403};
                                 }
                                 else
                                 {
                                     auto path_parse_result =
                                         Format{"/api%s", Format::Type::scanf}.parse(
                                             file.getContent()[HttpResponse::RequestKey::path]);
                                     if (path_parse_result)
                                     {
                                         auto api_path =
                                             std::get<std::string>((*path_parse_result)[0]);
                                         server.callback_callback(api_path, file);
                                     }
                                     else
                                     {
                                         file.write(HttpResponse{400});
                                     }
                                 }
                             });
    server.addCallbackFormat(
        Format{"/register", Format::Type::same},
        [](serverFile& file)
        {
            auto content = file.getContent()[HttpResponse::RequestKey::postcontent];
            auto ret =
                Format{"name = '%[^']', password = '%[^']'", Format::Type::scanf}.parse(content);
            if (!ret)
            {
                file << HttpResponse{400};
            }
            else
            {
                if (register_user(std::get<std::string>((*ret)[0]),
                                  std::get<std::string>((*ret)[1])))
                {
                    // file << HttpServerUtil::makeHttp(
                    //     200, std::format("your name is: {}, password is: {}",
                    //                      std::get<std::string>((*ret)[0]),
                    //                      std::get<std::string>((*ret)[1])));
                    file << HttpResponse::text(std::format("your name is: {}, password is: {}",
                                                           std::get<std::string>((*ret)[0]),
                                                           std::get<std::string>((*ret)[1])));
                }
                else
                {
                    file << HttpResponse{401};
                }
            }
        });
    server.addCallbackFormat(
        Format{"/deregistration", Format::Type::same},
        [](serverFile& file)
        {
            auto content = file.getContent()[HttpResponse::RequestKey::postcontent];
            auto ret =
                Format{"name = '%[^']', password = '%[^']'", Format::Type::scanf}.parse(content);
            if (!ret)
            {
                file << HttpResponse{400};
            }
            else
            {
                if (deregistration(std::get<std::string>((*ret)[0]),
                                   std::get<std::string>((*ret)[1])))
                {
                    file << HttpResponse::text("ok");
                }
                else
                {
                    file << HttpResponse{401};
                }
            }
        });

    auto coru = Co_Manager{};
    // coru.add(iol);
    coru.add(server);
    while (1)
    {
        coru.go();
    }
    return 0;
}
auto callbackfun(HttpClient& self)
{
    std::cout << "get!: \n";
    for (auto& each : self.getContent())
    {
        std::cout << each.first << ": " << each.second << "\n";
    }
    std::cout << "\n";
};
int main()
{
    threadpool tp{6};
    // tp.enqueue([]() { each_process(8081); });
    // tp.enqueue([]() { each_process(8080); });
    tp.enqueue(Debugger::network::TcpTest);
    tp.enqueue(
        [&tp]()
        {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cout << "start client!\n";
            HttpClient client{port(8080), "47.108.187.235"};

            client.setcallback(callbackfun);
            std::cout << "write socket\n";
            client.cilent_socket.writeFile(HttpRequst::GET("/"));

            while (client.eventGo() == EventStatus::Continue)
            {
            }
            std::cout << "quit\n";
        });
    // tp.enqueue(
    //     []()
    //     {
    //         // 创建端口转发器：8080 <-> 3000
    //         tcpForwarder forwarder(static_cast<port>(3000), static_cast<port>(8080), "127.0.0.1",
    //                                "0.0.0.0");
    //         // tcpForwarder forwarder(static_cast<port>(3000), static_cast<port>(8080));

    //         // 在事件循环中运行
    //         while (true)
    //         {
    //             forwarder.eventGo();
    //         }
    //     });
    tp.enqueue(
        [&]()
        {
            stdiolistener iol;
            auto co = Co_Manager{};
            co.add(iol);
            while (1)
            {
                co.go();
            }
        });
    return 0;
}
// get!:
// body: <!DOCTYPE html>
// <html lang="zh-CN">

// <head>
//     <meta charset="UTF-8">
//     <meta name="viewport" content="width=device-width, initial-scale=1.0">
//     <title>聊天应用</title>
//     <link rel="stylesheet" href="style.css">
// </head>

// <body>
//     <div class="container">
//         <!-- 登录表单 -->
//         <div id="loginForm" class="form-container">
//             <h2>登录</h2>
//             <form id="login">
//                 <div class="input-group">
//                     <input type="text" id="loginUsername" placeholder="用户名" required>
//                 </div>
//                 <div class="input-group">
//                     <input type="password" id="loginPassword" placeholder="密码" required>
//                 </div>
//                 <button type="submit">登录</button>
//                 <p>还没有账号？ <a href="#" onclick="showRegister()">立即注册</a></p>
//             </form>
//         </div>

//         <!-- 注册表单 -->
//         <div id="registerForm" class="form-container hidden">
//             <h2>注册</h2>
//             <form id="register">
//                 <div class="input-group">
//                     <input type="text" id="registerUsername" placeholder="用户名" required>
//                 </div>

// connection: keep-alive
// content-length: 3028
// content-type: text/html
// original_content: HTTP/1.1 200 OK
// Server: co_Http
// Content-Type: text/html
// Connection: keep-alive
// Content-Length: 3028

// <!DOCTYPE html>
// <html lang="zh-CN">

// <head>
//     <meta charset="UTF-8">
//     <meta name="viewport" content="width=device-width, initial-scale=1.0">
//     <title>聊天应用</title>
//     <link rel="stylesheet" href="style.css">
// </head>

// <body>
//     <div class="container">
//         <!-- 登录表单 -->
//         <div id="loginForm" class="form-container">
//             <h2>登录</h2>
//             <form id="login">
//                 <div class="input-group">
//                     <input type="text" id="loginUsername" placeholder="用户名" required>
//                 </div>
//                 <div class="input-group">
//                     <input type="password" id="loginPassword" placeholder="密码" required>
//                 </div>
//                 <button type="submit">登录</button>
//                 <p>还没有账号？ <a href="#" onclick="showRegister()">立即注册</a></p>
//             </form>
//         </div>

//         <!-- 注册表单 -->
//         <div id="registerForm" class="form-container hidden">
//             <h2>注册</h2>
//             <form id="register">
//                 <div class="input-group">
//                     <input type="text" id="registerUsername" placeholder="用户名" required>
//                 </div>

// reason_phrase: OK
// server: co_Http
// status_code: 200
// version: HTTP/1.1
