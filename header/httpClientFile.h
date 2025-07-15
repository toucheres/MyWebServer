#pragma once
#include "clientFile.hpp"
#include "corutine.hpp"
#include "file.h"
#include "port_fd.h"
#include <map>
#include <string>

class HttpRequst
{
  private:
    std::string http_version_ = "HTTP/1.1";
    std::string method_ = "GET";
    std::string path_ = "/";
    std::map<std::string, std::string> headers_;
    std::string body_;
    std::string result_cache_;
    bool chunked_mode_ = false;

  public:
    // 静态工厂方法
    static HttpRequst GET(const std::string& path);
    static HttpRequst POST(const std::string& path, const std::string& body = "");
    static HttpRequst PUT(const std::string& path, const std::string& body = "");
    static HttpRequst DELETE(const std::string& path);
    static HttpRequst text(const std::string& path, const std::string& content,
                           const std::string& method = "POST",
                           const std::string& content_type = "text/plain;charset=utf-8");
    static HttpRequst json(const std::string& path, const std::string& json_content,
                           const std::string& method = "POST");
    static HttpRequst form(const std::string& path,
                           const std::map<std::string, std::string>& form_data,
                           const std::string& method = "POST");

    // 构造函数
    HttpRequst(const std::string& path, const std::string& method = "GET");

    // 链式调用方法
    HttpRequst& addHeader(const std::string& key, const std::string& value);
    HttpRequst& setUserAgent(const std::string& user_agent);
    HttpRequst& setContentType(const std::string& content_type);
    HttpRequst& setAuthorization(const std::string& auth);
    HttpRequst& setCookie(const std::string& cookie);
    HttpRequst& setBody(const std::string& body, const std::string& content_type = "");
    HttpRequst& setJsonBody(const std::string& json);
    HttpRequst& setFormBody(const std::map<std::string, std::string>& form_data);

    // 分块传输相关
    HttpRequst& enableChunked();
    HttpRequst& addChunk(const std::string& chunk_data);
    HttpRequst& endChunked();

    // 获取方法
    const std::string& getMethod() const
    {
        return method_;
    }
    const std::string& getPath() const
    {
        return path_;
    }
    const std::string& getVersion() const
    {
        return http_version_;
    }
    const std::string& getBody() const
    {
        return body_;
    }

    // 转换为字符串
    operator std::string() const;
    std::string toString() const;

    // 常用请求类型常量
    class HttpRequestType
    {
      public:
        static constexpr const char* GET = "GET";
        static constexpr const char* POST = "POST";
        static constexpr const char* PUT = "PUT";
        static constexpr const char* DELETE = "DELETE";
        static constexpr const char* HEAD = "HEAD";
        static constexpr const char* OPTIONS = "OPTIONS";
        static constexpr const char* PATCH = "PATCH";
    };

    // 常用Content-Type常量
    class ContentType
    {
      public:
        static constexpr const char* TEXT_PLAIN = "text/plain;charset=utf-8";
        static constexpr const char* APPLICATION_JSON = "application/json;charset=utf-8";
        static constexpr const char* APPLICATION_FORM = "application/x-www-form-urlencoded";
        static constexpr const char* MULTIPART_FORM = "multipart/form-data";
        static constexpr const char* TEXT_HTML = "text/html;charset=utf-8";
        static constexpr const char* APPLICATION_XML = "application/xml;charset=utf-8";
    };

  private:
    // 辅助方法
    std::string urlEncode(const std::string& value) const;
    std::string formDataToString(const std::map<std::string, std::string>& form_data) const;
};
class HttpClientUtil
{
  public:
    static bool initialize();
    static bool autoRegistered;
    static Task<void, void> httpEventloop(clientFile* self);
    SocketFile createTcpClient(port port, std::string ip);
    class CilentKey
    {
      public:
        // HTTP响应状态行相关
        static constexpr const char* version = "version";
        static constexpr const char* status_code = "status_code";
        static constexpr const char* reason_phrase = "reason_phrase";

        // HTTP头部相关
        static constexpr const char* content_length = "content-length";
        static constexpr const char* content_type = "content-type";
        static constexpr const char* transfer_encoding = "transfer-encoding";
        static constexpr const char* connection = "connection";
        static constexpr const char* set_cookie = "set-cookie";
        static constexpr const char* location = "location";
        static constexpr const char* server = "server";
        static constexpr const char* date = "date";
        static constexpr const char* cache_control = "cache-control";
        static constexpr const char* expires = "expires";
        static constexpr const char* last_modified = "last-modified";
        static constexpr const char* etag = "etag";

        // CORS相关头部
        static constexpr const char* access_control_allow_origin = "access-control-allow-origin";
        static constexpr const char* access_control_allow_credentials =
            "access-control-allow-credentials";
        static constexpr const char* access_control_allow_methods = "access-control-allow-methods";
        static constexpr const char* access_control_allow_headers = "access-control-allow-headers";

        // 响应内容相关
        static constexpr const char* body = "body";
        static constexpr const char* original_content = "original_content";

        // 分块传输相关
        static constexpr const char* is_chunked = "is_chunked";
        static constexpr const char* chunked_complete = "chunked_complete";

        // 尾部头部前缀
        static constexpr const char* trailer_prefix = "trailer_";

        // keep-alive相关
        static constexpr const char* keep_alive = "keep_alive";
    };
};