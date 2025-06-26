#include "corutine.hpp"
#include "format.h"
#include "getmessage.hpp"
#include "http.h"
#include "httpServerFile.h"
#include "json.hpp"
#include "login.hpp"
#include "message.h"
#include "mysqlHandle.h"
#include "serverFile.h"
#include "stdiomanager.hpp"
#include "users.h"
#include "webSocketFile.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
HttpServer server;
LocalFiles LocalFileCache;
MySQLHandle sql{"localhost", "webserver", "WebServer@2025", "chat_db"};
int main()
{
    json::defultOpt.flatten_single_member = true;
    stdiolistener iol;
    iol.on_user_input.connect(
        [](std::string_view in)
        {
            std::cout << "get: " << in;
            auto handle = enable_sql<messages>(sql);
            if (in == "insert\n")
            {
                std::cout << "start insert\n";
                handle.insert(messages{.sender = "testsender", .content = "testcontent"});
            }
            else if (in == "del\n")
            {
                std::cout << "start del\n";
                auto ret_view = handle.select_where("sender = 'testsender'");
                std::cout << "before: \n";
                for (auto& each : ret_view)
                {
                    std::cout << json::from(each.to_struct()) << '\n';
                }
                std::cout << '\n';
                handle.remove("sender = 'testsender'");
                std::cout << "after: \n";
                auto ret_view_a = handle.select_where("sender = 'testsender'");
                for (auto& each : ret_view_a)
                {
                    std::cout << json::from(each.to_struct()) << '\n';
                }
                std::cout << '\n';
            }
        });
    server.autoLoginFile(LocalFileCache);
    server.addCallbackFormat(
        Format{"/login/%[^/]/%s", Format::Type::scanf},
        [](serverFile& connection)
        {
            auto ret = Format{"/login/%[^/]/%s", Format::Type::scanf}.parse(
                connection.getContent()[HttpServerUtil::ContentKey::path]);
            if (ret)
            {
                auto context = *ret;
                auto username = std::get<std::string>(context[0]);
                auto password = std::get<std::string>(context[1]);
                auto logined = login(username, password);
                if (logined.size())
                {
                    // 构建包含Set-Cookie头的HTTP响应
                    std::string cookie_value{reinterpret_cast<char*>(logined.data()),
                                             logined.size()};
                    std::string response_body = "Login successful";

                    // 手动构建HTTP响应，包含Set-Cookie头
                    std::string http_response = "HTTP/1.1 200 OK\r\n";
                    http_response += "Content-Type: text/plain\r\n";
                    http_response +=
                        "Content-Length: " + std::to_string(response_body.length()) + "\r\n";
                    http_response +=
                        "Set-Cookie: session=" + cookie_value + "; Path=/; HttpOnly\r\n";
                    http_response += "Access-Control-Allow-Credentials: true\r\n";
                    http_response += "Access-Control-Allow-Origin: http://localhost:8080\r\n";
                    http_response += "\r\n";
                    http_response += response_body;

                    connection.write(http_response);
                }
                else
                {
                    connection.write(HttpResponse{403});
                }
            }
            else
            {
                connection.write(HttpResponse{400});
            }
        });
    server.addCallbackFormat(
        Format{"/ws", Format::Type::same},
        [](serverFile& file)
        {
            // std::cout << "ok" << '\n';
            auto cookie = file.getContent()["cookie"];
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
                    // ...
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
                else if (WebSocketUtil::shouldbeUpdataToWS(file))
                {
                    file << WebSocketUtil::makeWebSocketHandshake(file);
                    file.upgradeProtocol(Protocol::WebSocket);
                    file.getContent()["inws"] = "ok";
                }
                else
                {
                    file << HttpResponse{404};
                }
            }
        });
    server.addCallbackFormat(
        Format{"/do", Format::Type::same},
        [](serverFile& file)
        {
            // std::cout << "ok" << '\n';
            auto cookie = file.getContent()[HttpServerUtil::ContentKey::cookie];
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

            // std::cout << "Extracted session in /do: " << session_cookie << std::endl;

            auto ret = check(session_cookie);
            if (!ret)
            {
                file << HttpResponse{403};
            }
            else
            {
                file << HttpServerUtil::makeHttp(
                    200, std::format("your name is: {}, your password is {}", ret->username.content,
                                     ret->password.content));
            }
        });
    server.addCallbackFormat(Format{"/getmessages", Format::Type::same},
                             [](serverFile& file)
                             {
                                 // std::cout << "ok" << '\n';
                                 auto cookie = file.getContent()["cookie"];
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

                                 // std::cout << "Extracted session: " << session_cookie << std::endl;

                                 auto ret = check(session_cookie);
                                 if (!ret)
                                 {
                                     file << HttpResponse{403};
                                 }
                                 else
                                 {
                                     auto ret = getmessage();
                                     std::string res;
                                     res.reserve(2048);
                                     for (auto& each : ret)
                                     {
                                         res += json::from(each.to_struct());
                                         res += '\n';
                                     }
                                     file << HttpServerUtil::makeHttp(200, res);
                                 }
                             });

    server.addCallbackFormat(
        Format{"/api%s", Format::Type::scanf},
        [](serverFile& file)
        {
            auto cookie = file.getContent()[HttpServerUtil::ContentKey::cookie];
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
            // auto cookie_ = file.getContent()[HttpServerUtil::ContentKey::cookie];
            auto ret = check(session_cookie);
            if (!ret)
            {
                file.write(HttpServerUtil::makeHttp(403, ""));
            }
            else
            {
                auto path_parse_result = Format{"/api%s", Format::Type::scanf}.parse(
                    file.getContent()[HttpServerUtil::ContentKey::path]);
                if (path_parse_result)
                {
                    auto api_path = std::get<std::string>((*path_parse_result)[0]);
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
            auto content = file.getContent()[HttpServerUtil::ContentKey::postcontent];
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
                    file << HttpServerUtil::makeHttp(
                        200, std::format("your name is: {}, password is: {}",
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
            auto content = file.getContent()[HttpServerUtil::ContentKey::postcontent];
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
                    file << HttpServerUtil::makeHttp(200, "ok");
                }
                else
                {
                    file << HttpResponse{401};
                }
            }
        });
    auto& coru = Co_Start_Manager::getInstance();
    coru.add(iol);
    coru.add(server);
    coru.start();
    return 0;
}

// struct in2
// {
//     std::string in2;
// };
// struct in
// {
//     std::string in;
// };
// struct test
// {
//     std::string test_;
//     in in_;
//     in2 in2_;
// };

// int main_()
// {
//     {
//         // json::defultOpt.flatten_single_member = false;

//         // auto obj = test{.in2_ = "1", .in_ = "2", .test_ = "3"};
//         // std::cout << json::from(obj) << '\n';
//         // std::cout << json::from(json::to<test>(json::from(obj))) << '\n';

//         // auto obj_ = users{.created_at = "123", .id = 12, .password = "we", .username =
//         // "sadf"};
//         // std::cout << json::from(obj_) << '\n';
//         // std::cout << json::from(json::to<users>(json::from(obj_))) << '\n';
//     }
//     {
//         // json::defultOpt.flatten_single_member = true;

//         // auto obj = test{.in2_ = "1", .in_ = "2", .test_ = "3"};
//         // std::cout << json::from(obj) << '\n';
//         // std::cout << json::from(json::to<test>(json::from(obj))) << '\n';
//         json::defultOpt.flatten_single_member = true;
//         json test;
//         // test.content = "{\"id\":0,\"username\":"
//         //                "\"testname\",\"password\":\"testpassword\",\"created_at\":\"testc\"}";
//         test.content = "";
//         auto ret = json::to<users>(test);
//         users u;
//         u.id = 0;
//         u.password = "12";
//         u.username = "123";

//         std::cout << test << '\n';
//         std::cout << json::from(u) << '\n';

//         std::cout << json::from(ret) << '\n';
//         std::cout << json::from(json::to<users>(json::from(ret))) << '\n';
//     }
// }