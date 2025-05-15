#include "serverFile.h"
#include "httpServerFile.h"
#include <algorithm>
#include <string>

// HTTP协议的事件循环 - 静态方法
Task<void, void> HttpServerUtil::httpEventloop(serverFile* self)
{
    // 解析状态相关变量
    ParseState state = REQUEST_LINE;
    std::string method;
    std::string path;
    std::string version;
    size_t content_length = 0;
    size_t body_read = 0;
    std::string body_buffer;

    while (self->fileState)
    {
        int ret = self->socketfile.eventGo();
        if (ret == -1)
        {
            self->fileState = false;
            co_yield {};
        }

        if ((!self->socketfile.handle.context) ||
            (self->socketfile.handle.context->fd_state == SocketFile::WRONG))
        {
            self->fileState = false;
            co_yield {};
        }

        std::string_view tp = self->socketfile.read_line();
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

                    self->content["method"] = method;
                    self->content["path"] = path;
                    self->content["version"] = version;

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
                    auto it = self->content.find("content-length");
                    if (it != self->content.end())
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

                    self->content.try_emplace(key, val);
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
                    self->content.try_emplace("postcontent", body_buffer);
                    state = COMPLETE;
                }
            }
            break;

        case COMPLETE:
        {
            if (self->socketfile.handle.context &&
                self->socketfile.handle.context->fd_state != SocketFile::WRONG)
            {
                self->handle();
                
                // 只重置必要的状态变量
                state = REQUEST_LINE;
                method.clear();
                path.clear();
                version.clear();
                content_length = 0;
                body_read = 0;
                body_buffer.clear();
            }
        }
        break;
        }

        co_yield {};
    }
    co_return;
}