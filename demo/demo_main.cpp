#include "corutine.hpp"
#include "format.h"
#include "http.h"
#include "httpServerFile.h"
#include "json.hpp"
#include "login.hpp"
#include "message.h"
#include "mysqlHandle.h"
#include "serverFile.h"
#include "stdiomanager.hpp"
#include "users.h"
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
                connection.getContent()["path"]);
            if (ret)
            {
                auto context = *ret;
                auto username = std::get<std::string>(context[0]);
                auto password = std::get<std::string>(context[1]);
                auto logined = login(username, password);
                if (logined.size())
                {
                    auto res = HttpServerUtil::makeHttpHead(
                        200,
                        std::string_view{reinterpret_cast<char*>(logined.data()), logined.size()});
                    connection.write(res);
                    connection.write(
                        std::string{reinterpret_cast<char*>(logined.data()), logined.size()});
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
    server.addCallbackFormat(Format{"/do", Format::Type::same},
                             [](serverFile& file)
                             {
                                 std::cout << "ok" << '\n';
                                 auto cookie = file.getContent()["cookie"];
                                 auto ret = check(cookie);
                                 file << HttpServerUtil::makeHttp(
                                     200, std::format("your name is: {}, your password is {}",
                                                      ret.username.content, ret.password.content));
                             });
    server.addCallbackFormat(
        Format{"/api%s", Format::Type::scanf},
        [](serverFile& file)
        {
            auto cookie = file.getContent()["cookie"];
            auto ret = check(cookie);
            // std::cout << file.socketfile_.handle_.context->content.data() << '\n';
            if (ret.username != "")
            {
                auto path_parse_result =
                    Format{"/api%s", Format::Type::scanf}.parse(file.getContent()["path"]);
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
            else
            {
                file.write(HttpServerUtil::makeHttp(403, ""));
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

// int main()
// {
//     {
//         // json::defultOpt.flatten_single_member = false;

//         // auto obj = test{.in2_ = "1", .in_ = "2", .test_ = "3"};
//         // std::cout << json::from(obj) << '\n';
//         // std::cout << json::from(json::to<test>(json::from(obj))) << '\n';

//         // auto obj_ = users{.created_at = "123", .id = 12, .password = "we", .username =
//         "sadf"};
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
//         test.content = "{\"id\":0,\"username\":"
//                        "\"testname\",\"password\":\"testpassword\",\"created_at\":\"testc\"}";
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