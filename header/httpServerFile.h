#pragma once
#include "serverFile.h"
#include <string>
#include <string_view>
#include <unordered_map>

// HTTP服务器工具类 - 类似WebSocketUtil
// std::string response = "";
// response.append("HTTP/1.1 ");
// response.append(std::to_string(status));
// if (status == 200)
//     response.append(" OK");
// else if (status == 404)
//     response.append(" Not Found");
// response.append("\r\n");
// response.append("Server: co_Http\r\n");
// response.append("Content-Type: ");
// response.append(std::string(content_type));
// response.append("\r\n");
// response.append("Connection: keep-alive\r\n");
// response.append("Content-Length: ");
// response.append(std::to_string(content.length()));
// response.append("\r\n\r\n");
// return response;
struct HttpResponse
{
    std::string content;
    HttpResponse& add(std::string key, std::string val);
    HttpResponse& with_content(std::string content, std::string type = "text/plain;charset=utf-8");
    HttpResponse(size_t status, std::string httptype = "HTTP/1.1",
                 std::string servername = default_servername);
    static HttpResponse formLocalFile(std::string path, std::string type);
    static HttpResponse formLocalFile(std::string path); // auto select type
    inline static std::string default_servername = "co_http";
    inline static std::map<size_t, std::string> status_num_string = {{200, "OK"}};
    operator std::string();
};

class HttpServerUtil
{
  private:
    // 添加自动注册HTTP协议处理函数的静态变量
    static bool autoRegistered;

  public:
    // HTTP解析状态枚举
    enum ParseState
    {
        REQUEST_LINE,
        HEADERS,
        BODY,
        COMPLETE
    };

    // HTTP事件循环 - 改为静态方法
    static Task<void, void> httpEventloop(serverFile* self);

    // 添加从HttpServer移动过来的HTTP协议相关函数
    static std::string makeHttpHead(int status, std::string_view content,
                                    std::string_view content_type = "text/plain;charset=utf-8");

    static std::string judge_file_type(std::string_view path);
    
    // 初始化方法，注册HTTP协议处理函数
    static bool initialize();

    // 禁止实例化
    HttpServerUtil() = delete;
    ~HttpServerUtil() = delete;
    HttpServerUtil(const HttpServerUtil&) = delete;
    HttpServerUtil& operator=(const HttpServerUtil&) = delete;
};
