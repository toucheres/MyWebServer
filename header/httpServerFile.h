#pragma once
#include "serverFile.h"
#include "protocol_constants.h" // 新增包含
#include <string>
#include <string_view>
#include "file.h" // 确保包含LocalFiles的定义

struct HttpResponse
{
  private:
    std::string http_version_ = "HTTP/1.1";
    size_t status_code_ = 0;
    std::string reason_phrase_ = "";
    std::map<std::string, std::string> headers_;
    std::string body_;

  public:
    HttpResponse& addHeader(std::string key, std::string val);
    HttpResponse& with_content(std::string content, std::string type = "text/plain;charset=utf-8");
    HttpResponse(size_t status, std::string httptype = "HTTP/1.1",
                 std::string servername = default_servername);
    static HttpResponse formLocalFile(std::string path, std::string type);
    static HttpResponse formLocalFile(std::string path); // auto select type
    
    // 添加静态文件缓存
    static LocalFiles& getFileCache();
    
    inline static std::string default_servername = "co_http";
    inline static std::map<size_t, std::string> status_num_string = {{200, "OK"}, {404, "Not Found"}};
    operator std::string();
};

class HttpServerUtil
{
  private:
    // 添加自动注册HTTP协议处理函数的静态变量
    static bool autoRegistered;

  public:
    // HTTP解析状态枚举
    enum class ParseState
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
    
    // 添加URL解码函数
    static std::string urlDecode(const std::string& encoded);

    // 禁止实例化
    HttpServerUtil() = delete;
    ~HttpServerUtil() = delete;
    HttpServerUtil(const HttpServerUtil&) = delete;
    HttpServerUtil& operator=(const HttpServerUtil&) = delete;
};
