#include "corutine.hpp"
#include "format.h"
#include "getmessage.hpp"
#include "http.h"
#include "httpServerFile.h"
#include "json.hpp"
#include "login.hpp"
#include "message.h"
#include "serverFile.h"
#include "stdiomanager.hpp"
#include "users.h"
#include "webSocketFile.h"
#include <cstdlib>
#include <string>
HttpServer server;
LocalFiles LocalFileCache;
int main()
{
    // std::cout << std::format("http://localhost:{}", server.getPort()) << '\n';
    json::defultOpt.flatten_single_member = true;
    stdiolistener iol;
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
                    std::string cookie_value{reinterpret_cast<char*>(logined.data()),
                                             logined.size()};
                    auto httpres =
                        HttpResponse{200}
                            .with_content("Login successful")
                            .addHeader("Set-Cookie",
                                       "session=" + cookie_value + "; Path=/; HttpOnly")
                            .addHeader("Access-Control-Allow-Credentials", "true")
                            .addHeader("Access-Control-Allow-Origin",
                                       std::format("http://localhost:{}", server.getPort()));

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
                                 auto cookie =
                                     file.getContent()[HttpServerUtil::ContentKey::cookie];
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

                                 // std::cout << "Extracted session: " << session_cookie <<
                                 // std::endl;

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
                                     // file << HttpServerUtil::makeHttp(200, res);
                                     file << HttpResponse::text(res);
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
                // file.write(HttpServerUtil::makeHttp(403, ""));
                file << HttpResponse{403};
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
                    // file << HttpServerUtil::makeHttp(200, "ok");
                    file << HttpResponse::text("ok");
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