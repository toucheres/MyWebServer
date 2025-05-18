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
#include <string>
#include <webSocketFile.h> // For Protocol::WebSocket

struct Message
{
    std::string sender = "";
    std::string content = "";
    // 2025-05-17 14:23:55
    std::string time = "1970-01-01 00:00::00";
    Message(std::string asformat)
    {
        //...
    }
    Message(std::vector<std::string> asformat)
    {
        //...
    }
    operator std::string()
    {
        return "sender: " + sender + '\n' + "content: " + content + '\n' + "time: " + time + '\n' +
               '\n';
    }
};
// std::map<int, serverFile*> wsconnecting;
auto httpServer = HttpServer{};
auto mysqldb = MySQLHandle{"localhost", "webserver", "WebServer@2025", "chat_db", 3306};
int main()
{
    LocalFiles static_files;
    control con;
    auto& coManagerInstance = Co_Start_Manager::getInstance(); // Renamed for clarity

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
    httpServer.addCallbackFormat(Format{"/getHistory", Format::Type::same},
                                 [](serverFile& socketfile)
                                 {
                                     if (socketfile.getAgreementType() ==
                                         Protocol::HTTP) // 使用 Protocol 枚举
                                     {
                                         auto res = mysqldb.query("select * from massge");
                                         if (!res)
                                         {
                                             std::cerr << mysqldb.getLastError() << '\n';
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
                    // wsconnecting.emplace(socketfile.getSocketFile().getfd(), &socketfile);
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
                auto pullmassges = Message{socketfile.getContent()["message"]};
                mysqldb.query("...");
                // for (auto each : wsconnecting)
                // {
                //     each.second->write(WebSocketResponse::text(pullmassges));
                // }
            }

            return;
        });

    // 执行MySQL示例
    // mysqlExample();

    coManagerInstance.getManager().add(httpServer);                 // Use getter for manager
    coManagerInstance.getManager().add(con);                        // Use getter for manager
    coManagerInstance.setLoopInterval(std::chrono::nanoseconds(0)); // Use setLoopInterval

    // 启动服务器
    coManagerInstance.start();

    return 0;
}