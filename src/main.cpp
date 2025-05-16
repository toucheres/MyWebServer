#include "main.hpp"
#include "format.h"
#include "httpServerFile.h" // 确保包含HttpServerUtil的头文件
#include "platform.h"
#include "serverFile.h"
#include <chrono>
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <iostream>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <string>
#include <webSocketFile.h>

int main()
{
    LocalFiles static_files;
    control con;
    auto& coManagerInstance = Co_Start_Manager::getInstance(); // Renamed for clarity
    auto httpServer = HttpServer{};
    // httpServer.addCallbackFormat(
    //     Format{"/", Format::Type::same},
    //     [](serverFile& socketfile)
    //     {
    //         if (socketfile.getAgreementType() == Agreement::HTTP)
    //         {
    //             if (WebSocketUtil::shouldbeUpdataToWS(socketfile))
    //             {
    //                 std::cout << "get websocket upgrade request\n";
    //                 // 获取请求中的Sec-WebSocket-Key
    //                 const auto& headers = socketfile.getContent();
    //                 auto key_it = headers.find("sec-websocket-key");
    //                 // 获取客户端发送的key
    //                 std::string clientKey = key_it->second;
    //                 // 使用WebSocketUtil的方法生成握手响应
    //                 std::string response = WebSocketUtil::makeWebSocketHandshake(clientKey);
    //                 // 发送握手响应
    //                 socketfile.write(response);
    //                 // 升级协议 - 不再创建新对象，而是修改当前对象的协议类型
    //                 socketfile.upgradeProtocol(Protocol::WebSocket); // Use enum
    //                 socketfile.getContent()["path"] = "/ws";
    //             }
    //             else
    //             {
    //                 // 使用原生write替代write_str_with_agreement
    //                 std::string message = "http not updata to websocket\r\n";
    //                 std::string header = HttpServerUtil::makeHttpHead(200, message); //
    //                 使用HttpServerUtil中的函数 socketfile.write(header);
    //                 socketfile.write(message);
    //             }
    //             return;
    //         }
    //         else if (socketfile.getAgreementType() == Protocol::WebSocket) // Use enum
    //         {
    //             auto res = std::move(WebSocketUtil::makeWebSocketFrame(
    //                 true, WebSocketUtil::WebSocketOpcode::TEXT, // Use enum
    //                 "socket readed!:" + socketfile.getContent().at("path")));
    //             std::cout << "callbacked!" << '\n';
    //             socketfile.write(res);
    //             return;
    //         }
    //         return;
    //     });

    // httpServer.addCallbackFormat(
    //     Format{"/ws", Format::Type::same},
    //     [&static_files](serverFile& socketfile)
    //     {
    //         auto& content = socketfile.getContent();
    //         std::cout << "WebSocket消息已收到!" << std::endl;
    //         // 打印客户端发送的消息(如果有)
    //         if (content.find("message") != content.end())
    //         {
    //             std::cout << "客户端消息: " << content["message"] << std::endl;
    //         }

    //         // 准备回复消息
    //         std::string replyText =
    //             "Server received: " +
    //             (content.find("message") != content.end() ? content["message"] : "no message");

    //         // 使用原生write替代write_str_with_agreement
    //         if (socketfile.getAgreementType() == Protocol::HTTP) // Use enum
    //         {
    //             std::string header = HttpServerUtil::makeHttpHead(200, replyText); //
    //             使用HttpServerUtil中的函数 socketfile.write(header); socketfile.write(replyText);
    //         }
    //         else if (socketfile.getAgreementType() == Protocol::WebSocket) // Use enum
    //         {
    //             std::string frame =
    //                 WebSocketUtil::makeWebSocketFrame(true, WebSocketUtil::WebSocketOpcode::TEXT, replyText); // Use enum
    //             socketfile.write(frame);
    //         }
    //     });
    httpServer.addCallbackFormat(Format{"/%s", Format::Type::scanf},
                                 [](serverFile& self)
                                 {
                                     std::string path = &self.getContent()["path"][1];
                                     auto file = HttpResponse::formLocalFile(path);
                                     self.write(std::string(file)); // Explicit cast to string
                                 });

    httpServer.addCallbackFormat(Format{"/nonexistent.html", Format::Type::same},
                                 [](serverFile& self)
                                 {
                                     std::string path = "404.html";
                                     auto file = HttpResponse::formLocalFile(path);
                                     self.write(std::string(file)); // Explicit cast
                                 });
    httpServer.addCallbackFormat(Format{"/", Format::Type::same},
                                 [](serverFile& self)
                                 {
                                     std::string path = "index.html";
                                     auto file = HttpResponse::formLocalFile(path);
                                     self.write(std::string(file)); // Explicit cast
                                 });

    coManagerInstance.getManager().add(httpServer); // Use getter for manager
    coManagerInstance.getManager().add(con); // Use getter for manager
    coManagerInstance.setLoopInterval(std::chrono::nanoseconds(0)); // Use setLoopInterval

    // 启动服务器
    coManagerInstance.start();

    return 0;
}