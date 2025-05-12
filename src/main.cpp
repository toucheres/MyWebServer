#include "main.hpp"
#include "httpServerFile.h"
#include <chrono>
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <webSocketFile.h>
int main()
{
    LocalFiles static_files;
    control con;
    auto& coManager = Co_Start_Manager::getInstance();
    auto httpServer = HttpServer{};
    httpServer.addCallbackFormat(
        Format{"/%s", Format::Type::scanf},
        [&static_files](serverFile& file)
        {
            auto parseResult =
                Format{"/%s", Format::Type::scanf}.parse(file.getContent().at("path"));
            std::string path = "404.html"; // Default path if (parseResult &&
            if (!parseResult->empty() && std::holds_alternative<std::string>((*parseResult)[0]))
            {
                path = std::get<std::string>((*parseResult)[0]);
            }
            auto& Localfile = static_files.get(path);
            std::string_view content = Localfile.read();
            if (content != "")
            {
                std::string head =
                    HttpServer::makeHttpHead(200, content, HttpServer::judge_file_type(path));
                file.write(std::move(head));
                file.write(std::move(std::string(content)));
                file.reset();
            }
            else
            {
                content = static_files.get("404.html").read();
                std::string head =
                    HttpServer::makeHttpHead(200, content, HttpServer::judge_file_type(path));
                // std::cout << head << '\n';
                file.write(std::move(head));
                file.write(std::move(std::string(content)));
                file.reset();
            }
        });
    httpServer.addCallbackFormat(Format{"/", Format::Type::same},
                                 [&static_files](serverFile& file)
                                 {
                                     auto& Localfile = static_files.get("index.html");
                                     std::string_view content = Localfile.read();
                                     if (content != "")
                                     {
                                         // std::cout << content.size() << '\n';
                                         std::string head = HttpServer::makeHttpHead(
                                             200, content,
                                             HttpServer::judge_file_type("index.html"));
                                         // std::cout << head;
                                         // std::cout << head << '\n';
                                         file.write(std::move(head));
                                         file.write(std::move(std::string(content)));
                                         file.reset();
                                     }
                                 });
    // httpServer.addCallbackFormat(
    //     Format{"/", Format::Type::same},
    //     [&httpServer](serverFile& socketfile)
    //     {
    //         if (WebSocketFile::shouldbeUpdataToWS(socketfile))
    //         {
    //             std::cout << "get websocket upgarde requst\n";
    //             socketfile.write("requst ok-------\n");
    //             try
    //             {
    //                 std::shared_ptr<WebSocketFile> websocketfile_ptr =
    //                     std::make_shared<WebSocketFile>(dynamic_cast<HttpServerFile&&>(socketfile));
    //                 // auto& con = websocketfile_ptr->getContent();
    //                 // for (auto& a : con)
    //                 // {
    //                 //     std::cout << a.first << ": " << a.second << '\n';
    //                 // }
    //                 // websocketfile_ptr;
    //                 httpServer.fileMap.erase(websocketfile_ptr->socketfile.handle.context->fd);
    //                 httpServer.fileMap[websocketfile_ptr->socketfile.handle.context->fd] =
    //                     websocketfile_ptr;
    //                 // socketfile = std::move(websocketfile_ptr);
    //             }
    //             catch (const std::bad_cast& e)
    //             {
    //                 std::cerr << e.what() << '\n';
    //             }
    //         }
    //         else
    //         {
    //             auto& content = socketfile.getContent();
    //             for (auto& a : content)
    //             {
    //                 std::cout << a.first << ": " << a.second << '\n';
    //             }
    //             std::cout << '\n';
    //         }
    //     });
    coManager.manager.add(httpServer);
    coManager.manager.add(con);
    coManager.loopTime = std::chrono::nanoseconds(0);
    coManager.start();
    return 0;
}