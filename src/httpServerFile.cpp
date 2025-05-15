#include "http.h"
#include "serverFile.h"
#include <httpServerFile.h>
#include <string>
#include <webSocketFile.h>

int HttpServerFile::eventGo()
{
    corutine.resume();
    return fileState;
}

void HttpServerFile::closeIt()
{
    this->socketfile.closeIt();
}

int HttpServerFile::getStatus()
{
    return this->fileState;
}

HttpServerFile::HttpServerFile(int fd, std::function<void(serverFile&)> a_callback)
    : serverFile(fd), callback(a_callback)
{
    // 初始时使用HTTP协议处理
    protocolType = Agreement::HTTP;
    corutine = httpEventloop();
}

void HttpServerFile::setCallback(std::function<void(serverFile&)> a_callback)
{
    callback = a_callback;
}

int HttpServerFile::handle()
{
    callback(*this);
    return 0;
}

void HttpServerFile::write(std::string file)
{
    return this->socketfile.writeFile(file);
}

std::map<std::string, std::string>& HttpServerFile::getContent()
{
    return content; // 直接返回基类的content
}

const std::map<std::string, std::string>& HttpServerFile::getContent() const
{
    return content; // 返回基类的const引用
}

int HttpServerFile::reset()
{
    if (protocolType == Agreement::HTTP)
    {
        // 重置HTTP协议状态
        content.clear();
        state = REQUEST_LINE;
        method.clear();
        path.clear();
        version.clear();
        content_length = 0;
        body_read = 0;
        body_buffer.clear();
    }

    return 0;
}

bool HttpServerFile::upgradeProtocol(int newProtocol)
{
    if (newProtocol == protocolType)
    {
        return true; // 已经是该协议，无需切换
    }

    protocolType = newProtocol; // 现在直接修改基类的protocolType
    return serverFile::resetCorutine(); // 调用父类的实现
}

bool HttpServerFile::resetCorutine()
{
    // 委托给父类实现
    return serverFile::resetCorutine();
}

// HTTP协议的事件循环
Task<void, void> HttpServerFile::httpEventloop()
{
    while (1)
    {
        int ret = socketfile.eventGo();
        if (ret == -1)
        {
            fileState = false;
            co_yield {};
        }

        if ((!socketfile.handle.context) ||
            (socketfile.handle.context->fd_state == SocketFile::WRONG))
        {
            fileState = false;
            co_yield {};
        }

        std::string_view tp = socketfile.read_line();
        switch (state)
        {
        case REQUEST_LINE:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (!tp.empty())
            {
                size_t first_space = tp.find(' ');
                size_t second_space = tp.find(' ', first_space + 1);

                if (first_space != std::string_view::npos && second_space != std::string_view::npos)
                {
                    method = std::string(tp.substr(0, first_space));
                    path = std::string(tp.substr(first_space + 1, second_space - first_space - 1));
                    version =
                        std::string(tp.substr(second_space + 1, tp.length() - second_space - 3));

                    content.try_emplace("method", method);
                    content.try_emplace("path", path);
                    content.try_emplace("version", version);

                    state = HEADERS;
                }
            }
            break;

        case HEADERS:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (tp == "\r\n")
            {
                if (method == "POST")
                {
                    auto it = content.find("content-length");
                    if (it != content.end())
                    {
                        try
                        {
                            content_length = std::stoul(std::string(it->second));
                            state = BODY;
                        }
                        catch (...)
                        {
                            content_length = 0;
                            state = COMPLETE;
                        }
                    }
                    else
                    {
                        state = COMPLETE;
                    }
                }
                else
                {
                    state = COMPLETE;
                }
            }
            else
            {
                size_t index = tp.find(": ");
                if (index != std::string_view::npos)
                {
                    std::string key(tp.substr(0, index));
                    std::transform(key.begin(), key.end(), key.begin(),
                                   [](unsigned char c) { return std::tolower(c); });

                    std::string val;
                    if (index + 2 < tp.length())
                    {
                        size_t val_len = tp.length() - (index + 2) - 2;
                        val = std::string(tp.substr(index + 2, val_len));
                    }

                    content.try_emplace(key, val);
                }
            }
            break;

        case BODY:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (body_read < content_length)
            {
                body_buffer.append(tp.data(), tp.length());
                body_read += tp.length();

                if (body_read >= content_length)
                {
                    content.try_emplace("postcontent", body_buffer);
                    state = COMPLETE;
                }
            }
            break;

        case COMPLETE:
        {
            if (socketfile.handle.context &&
                socketfile.handle.context->fd_state != SocketFile::WRONG)
            {
                handle();
                state = REQUEST_LINE;
            }
        }
        break;
        }

        co_yield {};
    }
    co_return;
}