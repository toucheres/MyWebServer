#include <chrono>
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <iostream>
#include <string>
#include <string_view>

int main()
{
    LocalFiles static_files;
    auto& coManager = Co_Start_Manager::getInstance();
    auto httpServer = HttpServer{};
    
    // 自动加载并注册所有文件
    httpServer.autoLoginFile(static_files);
    
    // 你也可以保留特定路径的手动回调
    httpServer.addCallback("/",
                           [&static_files](HttpFile& file)
                           {
                               auto path = file.content.at("path");
                               if (path == "/")
                               {
                                   path = "index.html";
                               }
                               else
                               {
                                   path = &path.data()[1];
                               }
                               auto& Localfile = static_files.get(path);
                               std::string_view content = Localfile.read();
                               if (content != "")
                               {
                                   std::cout << content.size() << '\n';//[Bug]恒为3177
                                   std::string head = HttpServer::makeHttpHead(
                                       200, content, HttpServer::judge_file_type(path));
                                    std::cout << head << '\n';
                                   file.socketfile.writeFile(std::move(head));
                                   file.socketfile.writeFile(std::move(std::string(content)));
                               }
                           });
    coManager.manager.add(httpServer);
    coManager.loopTime = std::chrono::nanoseconds(0);
    coManager.start();
    return 0;
}