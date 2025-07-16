#include "httpMessage.h"
#include "httpServerFile.h"
#include <sstream>
#include <iomanip>

// HttpResponse实现
HttpResponse HttpResponse::defult404 = HttpResponse{404};

HttpResponse::HttpResponse(size_t status, std::string httptype, std::string servername)
    : http_version_(std::move(httptype)), status_code_(status)
{
    if (status_num_string.count(status))
    {
        reason_phrase_ = status_num_string[status];
    }
    headers_["Server"] = std::move(servername);
    headers_["Connection"] = "keep-alive";
}

HttpResponse HttpResponse::text(std::string content, size_t status, std::string contenttype,
                                std::string httptype)
{
    return HttpResponse{status, httptype}.with_content(content, contenttype);
}

HttpResponse HttpResponse::binary(std::string content, size_t status, std::string contenttype,
                                  std::string httptype)
{
    return HttpResponse{status, httptype}.with_content(content, contenttype);
}

HttpResponse HttpResponse::fromFileCache(const LocalFile& file)
{
    if (file)
    {
        return HttpResponse{200}.with_content(std::string{file.read()},
                                              HttpServerUtil::judge_file_type(file.getPath()));
    }
    return defult404;
}

HttpResponse HttpResponse::formLocalFile(std::string path, std::string type)
{
    // 使用文件缓存获取文件内容
    auto& fileCache = getFileCache();
    LocalFile& file = fileCache.get(platform::fixPath(path));
    std::string_view file_content_view = file.read();

    if (file)
    {
        HttpResponse response(200);
        response.with_content(std::string(file_content_view), type);
        return response;
    }
    else
    {
        return defult404;
    }
}

HttpResponse HttpResponse::formLocalFile(std::string path)
{
    return formLocalFile(path, HttpServerUtil::judge_file_type(path));
}

HttpResponse& HttpResponse::addHeader(std::string key, std::string val)
{
    headers_[std::move(key)] = std::move(val);
    return *this;
}

HttpResponse& HttpResponse::with_content(std::string new_content, std::string type)
{
    body_ = std::move(new_content);
    headers_["Content-Type"] = std::move(type);
    headers_["Content-Length"] = std::to_string(body_.length());
    return *this;
}

HttpResponse& HttpResponse::enableChunked()
{
    chunked_mode_ = true;
    headers_["Transfer-Encoding"] = "chunked";
    headers_.erase("Content-Length");
    return *this;
}

HttpResponse& HttpResponse::addChunk(const std::string& chunk_data)
{
    if (chunked_mode_)
    {
        body_.append(HttpServerUtil::createChunkedResponse(chunk_data));
    }
    else
    {
        body_.append(chunk_data);
    }
    return *this;
}

HttpResponse& HttpResponse::endChunked()
{
    if (chunked_mode_)
    {
        body_.append(HttpServerUtil::createChunkedEnd());
    }
    return *this;
}

LocalFiles& HttpResponse::getFileCache()
{
    static LocalFiles fileCache;
    return fileCache;
}

HttpResponse::operator std::string()
{
    std::ostringstream oss;
    oss << http_version_ << " " << status_code_ << " " << reason_phrase_ << "\r\n";

    if (!chunked_mode_ && headers_.find("Content-Length") == headers_.end())
    {
        headers_["Content-Length"] = std::to_string(body_.length());
    }

    for (const auto& header : headers_)
    {
        oss << header.first << ": " << header.second << "\r\n";
    }
    oss << "\r\n";
    oss << body_;
    return oss.str();
}

// HttpRequst实现
HttpRequst::HttpRequst(const std::string& path, const std::string& method)
    : method_(method), path_(path)
{
    headers_["Host"] = "localhost";
    headers_["User-Agent"] = "HttpClient/1.0";
    headers_["Accept"] = "*/*";
    headers_["Connection"] = "keep-alive";
}

HttpRequst HttpRequst::GET(const std::string& path)
{
    return HttpRequst(path, HttpRequestType::GET);
}

HttpRequst HttpRequst::POST(const std::string& path, const std::string& body)
{
    HttpRequst request(path, HttpRequestType::POST);
    if (!body.empty())
    {
        request.setBody(body);
    }
    return request;
}

HttpRequst HttpRequst::PUT(const std::string& path, const std::string& body)
{
    HttpRequst request(path, HttpRequestType::PUT);
    if (!body.empty())
    {
        request.setBody(body);
    }
    return request;
}

HttpRequst HttpRequst::DELETE(const std::string& path)
{
    return HttpRequst(path, HttpRequestType::DELETE);
}

HttpRequst HttpRequst::text(const std::string& path, const std::string& content,
                            const std::string& method, const std::string& content_type)
{
    return HttpRequst(path, method).setBody(content, content_type);
}

HttpRequst HttpRequst::json(const std::string& path, const std::string& json_content,
                            const std::string& method)
{
    return HttpRequst(path, method).setJsonBody(json_content);
}

HttpRequst HttpRequst::form(const std::string& path,
                            const std::map<std::string, std::string>& form_data,
                            const std::string& method)
{
    return HttpRequst(path, method).setFormBody(form_data);
}

HttpRequst& HttpRequst::addHeader(const std::string& key, const std::string& value)
{
    headers_[key] = value;
    return *this;
}

HttpRequst& HttpRequst::setUserAgent(const std::string& user_agent)
{
    headers_["User-Agent"] = user_agent;
    return *this;
}

HttpRequst& HttpRequst::setContentType(const std::string& content_type)
{
    headers_["Content-Type"] = content_type;
    return *this;
}

HttpRequst& HttpRequst::setAuthorization(const std::string& auth)
{
    headers_["Authorization"] = auth;
    return *this;
}

HttpRequst& HttpRequst::setCookie(const std::string& cookie)
{
    headers_["Cookie"] = cookie;
    return *this;
}

HttpRequst& HttpRequst::setBody(const std::string& body, const std::string& content_type)
{
    body_ = body;
    if (!content_type.empty())
    {
        setContentType(content_type);
    }
    if (!chunked_mode_)
    {
        headers_["Content-Length"] = std::to_string(body_.length());
    }
    return *this;
}

HttpRequst& HttpRequst::setJsonBody(const std::string& json)
{
    return setBody(json, ContentType::APPLICATION_JSON);
}

HttpRequst& HttpRequst::setFormBody(const std::map<std::string, std::string>& form_data)
{
    std::string form_string = formDataToString(form_data);
    return setBody(form_string, ContentType::APPLICATION_FORM);
}

HttpRequst& HttpRequst::enableChunked()
{
    chunked_mode_ = true;
    headers_["Transfer-Encoding"] = "chunked";
    headers_.erase("Content-Length");
    return *this;
}

HttpRequst& HttpRequst::addChunk(const std::string& chunk_data)
{
    if (chunked_mode_)
    {
        body_.append(HttpServerUtil::createChunkedResponse(chunk_data));
    }
    else
    {
        body_.append(chunk_data);
    }
    return *this;
}

HttpRequst& HttpRequst::endChunked()
{
    if (chunked_mode_)
    {
        body_.append(HttpServerUtil::createChunkedEnd());
    }
    return *this;
}

HttpRequst::operator std::string() const
{
    return toString();
}

std::string HttpRequst::toString() const
{
    std::ostringstream oss;
    oss << method_ << " " << path_ << " " << http_version_ << "\r\n";

    if (!chunked_mode_ && !body_.empty() && headers_.find("Content-Length") == headers_.end())
    {
        const_cast<HttpRequst*>(this)->headers_["Content-Length"] = std::to_string(body_.length());
    }

    for (const auto& header : headers_)
    {
        oss << header.first << ": " << header.second << "\r\n";
    }

    oss << "\r\n";
    oss << body_;
    return oss.str();
}

std::string HttpRequst::urlEncode(const std::string& value) const
{
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;

    for (char c : value)
    {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded << c;
        }
        else
        {
            encoded << '%' << std::setw(2) << (unsigned char)c;
        }
    }

    return encoded.str();
}

std::string HttpRequst::formDataToString(const std::map<std::string, std::string>& form_data) const
{
    std::ostringstream oss;
    bool first = true;

    for (const auto& pair : form_data)
    {
        if (!first)
        {
            oss << "&";
        }
        oss << urlEncode(pair.first) << "=" << urlEncode(pair.second);
        first = false;
    }

    return oss.str();
}
