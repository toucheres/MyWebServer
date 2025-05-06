#include <chrono>
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <iostream>

int main()
{
    LocalFiles static_files;
    auto& coManager = Co_Start_Manager::getInstance();
    auto httpServer = HttpServer{};
    httpServer.addCallback("/",
                           [&static_files](HttpFile& file)
                           {
                               //    auto path = file.content.at("path");
                               //    if (path == "/")
                               //    {
                               //        path = "index.html";
                               //    }
                               //    auto& Localfile = static_files.get(path);
                               //    auto content = Localfile.read();
                               //    if (content != "")
                               //    {
                               //        file.socketfile.writeFile(content);
                               //    }
                               std::string response_1 = "HTTP/1.1 200 OK\r\n"
                                                        "Content-Type: text/plain\r\n"
                                                        "Content-Length: 25\r\n";
                               std::string response_2 = "Connection: keep-alive\r\n"
                                                        "\r\n"
                                                        "Hello from MyWebServer!\r\n";
                               file.socketfile.writeFile(std::move(response_1));
                               file.socketfile.writeFile(std::move(response_2));
                           });
    coManager.manager.add(httpServer);
    coManager.loopTime = std::chrono::nanoseconds(0);
    coManager.start();
    return 0;
}