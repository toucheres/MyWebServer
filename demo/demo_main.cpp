#include "clientFile.hpp"
#include "corutine.hpp"
#include "file.h"
#include "format.h"
#include "getmessage.hpp"
#include "http.h"
#include "json.hpp"
#include "login.hpp"
#include "message.h"
#include "protocol_constants.h"
#include "serverFile.h"
#include "threadpool.hpp"
#include "users.h"
#include "webSocketClientFile.hpp"
#include "webSocketServerFile.h"
#include <cstdarg>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include "httpMessage.h"
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
                connection.getContent()[HttpRequst::CilentKey::path]);
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
            auto cookie = file.getContent()[HttpRequst::CilentKey::cookie];
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
                    auto mes = file.getContent()[WebSocketResponse::ContentKey::message];
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
                else if (WebSocketServerUtil::tryUpToWs(file))
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
                                 auto cookie = file.getContent()[HttpRequst::CilentKey::cookie];
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
                                 auto cookie = file.getContent()[HttpRequst::CilentKey::cookie];
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
                                 auto cookie = file.getContent()[HttpRequst::CilentKey::cookie];
                                 // std::cout << "Received cookie in /do: " << cookie << std::endl;
                                 // std::cout <<
                                 // file.getContent()[HttpRequst::CilentKey::orignal_content] <<
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
                                             file.getContent()[HttpRequst::CilentKey::path]);
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
            auto content = file.getContent()[HttpRequst::CilentKey::postcontent];
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
            auto content = file.getContent()[HttpRequst::CilentKey::postcontent];
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
auto callbackfun(clientFile& self)
{
    std::cout << "get!: \n";
    for (auto& each : self.getContent())
    {
        std::cout << each.first << ": " << each.second << "\n";
    }
    std::cout << "\n";
};
struct testclass
{
    int age;
    std::string name;
    bool operator<(const testclass& other) const
    {
        return age < other.age;
    }
};
int main()
{
    threadpool tp{6};
    // tp.enqueue([]() { Debugger::network::TcpTest((port)8080); });
    // tp.enqueue([]() { each_process(8081); });
    tp.enqueue(
        []()
        {
            HttpServer server{8081, "127.0.0.1"};
            server.addCallbackFormat(
                Format{"/", Format::Type::same},
                [](serverFile& self)
                {
                    if (self.getAgreementType() == Protocol::HTTP)
                    {
                        if (WebSocketServerUtil::tryUpToWs(self))
                        {
                            std::cout << "server get upws requset, and responsed!\n";
                        }
                        else
                        {
                            std::cout << "http get: " << '\n';
                            std::cout
                                << self.getContent()[HttpRequst::CilentKey::orignal_content]
                                << '\n';
                        }
                    }
                    else
                    {
                        std::cout << "ws server get: " << '\n';

                        std::cout << self.getContent()[WebSocketResponse::ContentKey::message]
                                  << '\n';
                    }
                });
            Co_Manager ma{};
            ma.add(server);
            while (1)
            {
                ma.go();
            }
        });
    tp.enqueue(
        []()
        {
            std::cout << "start client!\n";
            clientFile client{port(8081), "127.0.0.1"};
            std::cout << "write socket\n";
            client << WebSocketClientUtil::makeWebSocketHandshake();
            client.setcallback(
                [](clientFile& self)
                {
                    std::cout << "client get reply!\n";
                    if (self.getAgreementType() == Protocol::HTTP)
                    {
                        if (WebSocketClientUtil::shouldbeUpdataToWS(self))
                        {
                            self.upgradeProtocol(Protocol::WebSocket);
                            self << WebSocketRequst::text("ciallo from ws client!\n");
                            self.setcallback(
                                [](clientFile& self_ws)
                                {
                                    if (self_ws.getAgreementType() == Protocol::WebSocket)
                                    {
                                        std::cout << "ws client get:"
                                                  << self_ws.getContent()
                                                         [WebSocketClientUtil::ContentKey::message]
                                                  << '\n';
                                    }
                                });
                        }
                    }
                });
            while (client.eventGo() == EventStatus::Continue)
            {
            }
            std::cout << "quit\n";
        });
    return 0;
}
