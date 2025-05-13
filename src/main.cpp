#include "main.hpp"
#include "httpServerFile.h"
#include <chrono>
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <iostream>
#include <memory>
#include <openssl/bio.h>    // 包含 BIO 相关函数
#include <openssl/buffer.h> // 包含 BUF_MEM 结构体
#include <openssl/evp.h>
#include <openssl/sha.h> // 包含 SHA1() 和 SHA_DIGEST_LENGTH
#include <string>
#include <webSocketFile.h>
int main()
{
    LocalFiles static_files;
    control con;
    auto& coManager = Co_Start_Manager::getInstance();
    auto httpServer = HttpServer{};
    // httpServer.addCallbackFormat(
    //     Format{"/%s", Format::Type::scanf},
    //     [&static_files](serverFile& file)
    //     {
    //         auto parseResult =
    //             Format{"/%s", Format::Type::scanf}.parse(file.getContent().at("path"));
    //         std::string path = "404.html"; // Default path if (parseResult &&
    //         if (!parseResult->empty() && std::holds_alternative<std::string>((*parseResult)[0]))
    //         {
    //             path = std::get<std::string>((*parseResult)[0]);
    //         }
    //         auto& Localfile = static_files.get(path);
    //         std::string_view content = Localfile.read();
    //         if (content != "")
    //         {
    //             std::string head =
    //                 HttpServer::makeHttpHead(200, content, HttpServer::judge_file_type(path));
    //             file.write(std::move(head));
    //             file.write(std::move(std::string(content)));
    //             file.reset();
    //         }
    //         else
    //         {
    //             content = static_files.get("404.html").read();
    //             std::string head =
    //                 HttpServer::makeHttpHead(200, content, HttpServer::judge_file_type(path));
    //             // std::cout << head << '\n';
    //             file.write(std::move(head));
    //             file.write(std::move(std::string(content)));
    //             file.reset();
    //         }
    //     });
    // httpServer.addCallbackFormat(Format{"/", Format::Type::same},
    //                              [&static_files](serverFile& file)
    //                              {
    //                                  auto& Localfile = static_files.get("index.html");
    //                                  std::string_view content = Localfile.read();
    //                                  if (content != "")
    //                                  {
    //                                      // std::cout << content.size() << '\n';
    //                                      std::string head = HttpServer::makeHttpHead(
    //                                          200, content,
    //                                          HttpServer::judge_file_type("index.html"));
    //                                      // std::cout << head;
    //                                      // std::cout << head << '\n';
    //                                      file.write(std::move(head));
    //                                      file.write(std::move(std::string(content)));
    //                                      file.reset();
    //                                  }
    //                              });
    httpServer.addCallbackFormat(
        Format{"/", Format::Type::same},
        [&httpServer](serverFile& socketfile)
        {
            // WebSocket 升级响应处理
            if (socketfile.getAgreementType() == Agreement::HTTP)
            {
                if (HttpServerFile::shouldbeUpdataToWS(socketfile))
                {
                    std::cout << "get websocket upgrade request\n";
                    // 获取请求中的Sec-WebSocket-Key
                    const auto& headers = socketfile.getContent();
                    auto key_it = headers.find("sec-websocket-key");
                    if (key_it == headers.end() || key_it->second.empty())
                    {
                        socketfile.write("HTTP/1.1 400 Bad Request\r\n\r\n");
                        return;
                    }
                    // 获取客户端发送的key
                    std::string clientKey = key_it->second;
                    // 使用WebSocketFile的方法生成握手响应
                    std::string response = WebSocketFile::createWebSocketHandshake(clientKey);
                    // 发送握手响应
                    socketfile.write(response);
                    // 升级协议 - 不再创建新对象，而是修改当前对象的协议类型
                    socketfile.upgradeProtocol(Agreement::WebSocket);
                }
                else
                {
                    socketfile.write("http not updata to websocket\r\n");
                }
            }
            else if (socketfile.getAgreementType() == Agreement::WebSocket)
            {

                auto res = std::move(WebSocketFile::createWebSocketFrame(
                    true, WebSocketFile::TEXT,
                    "socket readed!:" + socketfile.getContent().at("path")));
                socketfile.write(res);
                socketfile.reset();
            }
        });
    coManager.manager.add(httpServer);
    coManager.manager.add(con);
    coManager.loopTime = std::chrono::nanoseconds(0);
    coManager.start();
    return 0;
}