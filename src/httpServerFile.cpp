#include <httpServerFile.h>
int HttpServerFile::eventGo()
{
    corutine.resume();
    return httpState;
}

void HttpServerFile::closeIt()
{
    this->socketfile.closeIt();
}

HttpServerFile::HttpServerFile(int fd, std::function<void(HttpServerFile&)> a_callback)
    : socketfile(fd), callback(a_callback)
{
}

int HttpServerFile::handle()
{
    callback(*this);
    // std::cout << "Headers count: " << content.size() << "\n";
    // std::cout << "path: " << content.at("path") << "\n";
    reset();
    return 0;
}

void HttpServerFile::write(std::string file)
{
    return this->socketfile.writeFile(file);
}

const std::map<std::string, std::string>& HttpServerFile::getContent()
{
    return content;
}

void HttpServerFile::reset()
{
    content.clear();
    state = REQUEST_LINE;
    method.clear();
    path.clear();
    version.clear();
    content_length = 0;
    body_read = 0;
    body_buffer.clear();
}

Task<void, void> HttpServerFile::eventloop()
{
    while (1)
    {
        int ret = socketfile.eventGo();
        if (ret == -1)
        {
            httpState = false;
            co_yield {};
        }

        if ((!socketfile.handle.context) ||
            (socketfile.handle.context->fd_state == SocketFile::WRONG))
        {
            httpState = false;
            // std::cout << "连接已关闭: " << socketfile.handle.context->fd << std::endl;
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
                // std::cout << "fd: " << socketfile.handle.context.get()->fd << " 请求行: " << tp;

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