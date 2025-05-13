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
            if (WebSocketFile::shouldbeUpdataToWS(socketfile))
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

                // 计算Sec-WebSocket-Accept
                // 将key与魔术字符串连接
                std::string combined = clientKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

                // 计算SHA1哈希
                unsigned char hash[SHA_DIGEST_LENGTH];
                SHA1(reinterpret_cast<const unsigned char*>(combined.c_str()), combined.length(),
                     hash);

                // Base64编码
                BIO* b64 = BIO_new(BIO_f_base64());
                BIO* bmem = BIO_new(BIO_s_mem());
                b64 = BIO_push(b64, bmem);
                BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
                BIO_write(b64, hash, SHA_DIGEST_LENGTH);
                BIO_flush(b64);

                BUF_MEM* bptr;
                BIO_get_mem_ptr(b64, &bptr);
                std::string acceptKey(bptr->data, bptr->length);
                BIO_free_all(b64);

                // 构建WebSocket握手响应
                std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
                                       "Upgrade: websocket\r\n"
                                       "Connection: Upgrade\r\n"
                                       "Sec-WebSocket-Accept: " +
                                       acceptKey + "\r\n\r\n";

                // 发送握手响应
                socketfile.write(response);

                try
                {
                    std::shared_ptr<WebSocketFile> websocketfile_ptr =
                        std::make_shared<WebSocketFile>(dynamic_cast<HttpServerFile&&>(socketfile));

                    httpServer.fileMap.erase(websocketfile_ptr->socketfile.handle.context->fd);
                    httpServer.fileMap[websocketfile_ptr->socketfile.handle.context->fd] =
                        websocketfile_ptr;

                    // 这里不需要再次写入数据，握手已完成
                    // websocketfile_ptr->write("");
                }
                catch (const std::bad_cast& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
        });
    coManager.manager.add(httpServer);
    coManager.manager.add(con);
    coManager.loopTime = std::chrono::nanoseconds(0);
    coManager.start();
    return 0;
}