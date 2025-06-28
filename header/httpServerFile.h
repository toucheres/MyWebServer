#pragma once
#include "file.h"               // 确保包含LocalFiles的定义
#include "serverFile.h"
#include <string>
#include <string_view>

struct HttpResponse
{
  private:
    std::string http_version_ = "HTTP/1.1";
    size_t status_code_ = 0;
    std::string reason_phrase_ = "";
    std::map<std::string, std::string> headers_;
    std::string body_;
    bool chunked_mode_ = false; // 分块传输模式标志
    static HttpResponse defult404;

  public:
    static HttpResponse text(std::string content, size_t status = 200,
                             std::string contenttype = "text/plain;charset=utf-8",
                             std::string httptype = "HTTP/1.1");
    static HttpResponse binary(std::string content, size_t status = 200,
                               std::string contenttype = "application/octet-stream",
                               std::string httptype = "HTTP/1.1");
    static HttpResponse fromFileCache(const LocalFile& file);
    static HttpResponse formLocalFile(std::string path, std::string type);
    static HttpResponse formLocalFile(std::string path); // auto select type
    HttpResponse& addHeader(std::string key, std::string val);
    // HttpResponse& addHeader(const std::string_view key, std::string val);
    HttpResponse& with_content(std::string content, std::string type = "text/plain;charset=utf-8");
    HttpResponse(size_t status, std::string httptype = "HTTP/1.1",
                 std::string servername = default_servername);
    // 分块传输相关方法
    HttpResponse& enableChunked();
    HttpResponse& addChunk(const std::string& chunk_data);
    HttpResponse& endChunked();

    static LocalFiles& getFileCache();

    inline static std::string default_servername = "co_http";
    inline static std::map<size_t, std::string> status_num_string = {{200, "OK"},
                                                                     {404, "Not Found"}};
    operator std::string();
    class RequestKey
    {
      public:
        static constexpr const char* method = "method";
        static constexpr const char* path = "path";
        static constexpr const char* version = "version";
        static constexpr const char* content_length = "content-length";
        static constexpr const char* postcontent = "postcontent";
        static constexpr const char* cookie = "cookie";
    };
    class ResponseKey
    {
      public:
        static constexpr const char* Set_Cookie = "Set-Cookie";
        static constexpr const char* Access_Control_Allow_Origin = "Access-Control-Allow-Origin";
        static constexpr const char* Access_Control_Allow_Credentials =
            "Access-Control-Allow-Credentials";
    };
};

class HttpServerUtil
{
  private:
    static bool autoRegistered;

  public:
    // HTTP解析状态枚举
    enum class ParseState
    {
        REQUEST_LINE,
        HEADERS,
        BODY,
        CHUNKED_SIZE,    // 解析分块大小
        CHUNKED_DATA,    // 解析分块数据
        CHUNKED_TRAILER, // 解析分块尾部
        COMPLETE
    };

    // HTTP事件循环
    static Task<void, void> httpEventloop(serverFile* self);

    // HTTP协议相关函数
    static std::string makeHttpHead(int status, std::string_view content,
                                    std::string_view content_type = "text/plain;charset=utf-8");
    static std::string makeHttp(int status, std::string_view content,
                                std::string_view content_type = "text/plain;charset=utf-8");
    static std::string judge_file_type(std::string_view path);
    static bool initialize();
    static std::string urlDecode(const std::string& encoded);

    // 分块传输相关的静态方法
    static std::string createChunkedResponse(const std::string& data);
    static std::string createChunkedEnd();
    static size_t parseChunkSize(const std::string& chunkSizeLine);

    // 新增：客户端分块传输检测和处理
    static bool isChunkedRequest(const std::map<std::string, std::string>& headers);
    static std::string createChunkedRequest(const std::string& method, const std::string& path,
                                            const std::map<std::string, std::string>& headers = {});
    static std::string addChunkToRequest(const std::string& chunk_data);
    static std::string endChunkedRequest();

    // 禁止实例化
    HttpServerUtil() = delete;
    ~HttpServerUtil() = delete;
    HttpServerUtil(const HttpServerUtil&) = delete;
    HttpServerUtil& operator=(const HttpServerUtil&) = delete;
};
