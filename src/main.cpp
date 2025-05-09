#include "main.hpp"
#include <chrono>
#include <corutine.hpp>
#include <file.h>
#include <http.h>
#include <string>
#include <string_view>
int main()
{
    LocalFiles static_files;
    control con;
    auto& coManager = Co_Start_Manager::getInstance();
    // auto httpServer = HttpServer{};
    // httpServer.addCallbackFormat(
    //     Format{"/%s", Format::Type::scanf},
    //     [&static_files](HttpServerFile& file)
    //     {
    //         auto parseResult = Format{"/%s", Format::Type::scanf}.parse(file.content.at("path"));
    //         std::string path = "404.html"; // Default path
    //         if (parseResult && !parseResult->empty() &&
    //             std::holds_alternative<std::string>((*parseResult)[0]))
    //         {
    //             path = std::get<std::string>((*parseResult)[0]);
    //         }
    //         auto& Localfile = static_files.get(path);
    //         std::string_view content = Localfile.read();
    //         if (content != "")
    //         {
    //             std::string head =
    //                 HttpServer::makeHttpHead(200, content, HttpServer::judge_file_type(path));
    //             file.socketfile.writeFile(std::move(head));
    //             file.socketfile.writeFile(std::move(std::string(content)));
    //         }
    //         else
    //         {
    //             content = static_files.get("404.html").read();
    //             std::string head =
    //                 HttpServer::makeHttpHead(200, content, HttpServer::judge_file_type(path));
    //             // std::cout << head << '\n';
    //             file.socketfile.writeFile(std::move(head));
    //             file.socketfile.writeFile(std::move(std::string(content)));
    //         }
    //     });
    // httpServer.addCallbackFormat(Format{"/", Format::Type::same},
    //                              [&static_files](HttpServerFile& file)
    //                              {
    //                                  auto& Localfile = static_files.get("index.html");
    //                                  std::string_view content = Localfile.read();
    //                                  if (content != "")
    //                                  {
    //                                      // std::cout << content.size() << '\n';
    //                                      std::string head = HttpServer::makeHttpHead(
    //                                          200, content,
    //                                          HttpServer::judge_file_type("index.html"));
    //                                      // std::cout << head << '\n';
    //                                      file.socketfile.writeFile(std::move(head));
    //                                      file.socketfile.writeFile(std::move(std::string(content)));
    //                                  }
    //                              });
    // coManager.manager.add(httpServer);
    coManager.manager.add(con);
    coManager.loopTime = std::chrono::nanoseconds(0);
    coManager.start();
    return 0;
}