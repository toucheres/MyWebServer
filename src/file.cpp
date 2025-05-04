#include <errno.h>
#include <fcntl.h>
#include <file.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string.h>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>

// LocalFile实现
LocalFile::LocalFile(std::string a_path) : path(std::move(a_path)), size(0)
{
    load(path);
}

LocalFile::LocalFile(LocalFile&& move)
    : content(std::move(move.content)), path(std::move(move.path)), size(move.size), test(move.test)
{
    move.size = 0;
}

bool LocalFile::load(std::string& path)
{
    this->path = path;
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return false;
    }

    size = file.tellg();
    file.seekg(0, std::ios::beg);

    content.resize(size);
    if (size > 0)
    {
        file.read(content.data(), size);
    }

    test = std::string_view(content.data(), size);
    return true;
}

std::string_view LocalFile::read()
{
    return test;
}

// SocketFile实现
SocketFile::SocketFile(int a_fd)
{
    load(a_fd);
    setNonBlocking();
}

SocketFile::~SocketFile()
{
    // 防止临时变量析构后close
    if (handle.context != nullptr)
    {
        close(handle.context->fd);
    }
}

SocketFile::SocketFile(SocketFile&& move) : handle(std::move(move.handle))
{
    move.handle.context = nullptr;
}

SocketFile& SocketFile::operator=(SocketFile&& move)
{
    this->handle = std::move(move.handle);
    return *this;
}

// SocketFile::SocketFile(const SocketFile& copy)
//     : handle(copy.handle) {
// }

bool SocketFile::load(int a_fd)
{
    handle.get_context()->fd = a_fd;
    handle.get_context()->r_left = 0;
    handle.get_context()->r_right = 0;
    handle.get_context()->content.resize(4096); // 预分配缓冲区
    return true;
}

Task<> SocketFile::eventfun(std::shared_ptr<CONTEXT> context)
{
    while (true)
    {
        // 检查缓冲区是否需要扩容
        if (context->r_right >= context->content.size())
        {
            context->content.resize(context->content.size() * 2);
        }
        // 异步读取数据
        int flags = fcntl(context->fd, F_GETFL, 0);
        if (flags == -1)
        {
            std::cerr << "获取socket标志失败: " << strerror(errno) << std::endl;
        }
        if (fcntl(context->fd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            std::cerr << "设置非阻塞模式失败: " << strerror(errno) << std::endl;
        }
        ssize_t n = read(context->fd, context->content.data() + context->r_right,
                         context->content.size() - context->r_right);

        if (n > 0)
        {
            context->r_right += n;
        }
        else if (n == 0)
        {
            // EOF 文件结束
            context->fd_state = WRONG;
            co_yield {};
        }
        else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            // 暂无数据可读，暂停协程
            // co_yield {};
        }
        else
        {
            // 读取错误
            if (errno == ECONNRESET || errno == EPIPE || errno == ECONNABORTED)
            {
                std::cerr << "客户端断开连接: " << strerror(errno) << ", fd: " << context->fd
                          << std::endl;
            }
            else
            {
                std::cerr << "Socket read error: " << strerror(errno) << ", fd: " << context->fd
                          << std::endl;
            }
            context->fd_state = WRONG;
            co_yield {};
        }
        // 写数据
        if (context->w_right > context->w_left)
        {
            int written = ::write(context->fd, &context->waitingWrite[context->w_left],
                                  context->w_right - context->w_left);
            if (written == -1)
            {
                // 处理写入错误
                if (errno == EINTR)
                {
                    // 被信号中断，继续尝试
                    continue;
                }
                else if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // 非阻塞模式下缓冲区已满，可以稍后重试
                    // 在协程环境中，应该yield后继续
                    // co_yield {};
                    // continue;
                }
                else
                {
                    // 其他错误（连接关闭等）
                    std::cerr << "写入失败: " << strerror(errno) << std::endl;
                    context->fd_state = WRONG;
                    break;
                }
            }
            else if (written == 0)
            {
                // 连接已关闭
                std::cerr << "连接已关闭" << std::endl;
                context->fd_state = WRONG;
                break;
            }
            else
            {
                context->w_left += written;
            }
        }
        co_yield {};
    }
}

int SocketFile::eventGo()
{
    if (!handle.context || handle.context->fd_state == WRONG)
    {
        return -1;
    }
    handle.resume();
    return 0;
}

const std::string_view SocketFile::read_added() const
{
    if (handle.get_context()->r_left >= handle.get_context()->r_right)
    {
        return std::string_view();
    }

    std::string_view result(handle.get_context()->content.data() + handle.get_context()->r_left,
                            handle.get_context()->r_right - handle.get_context()->r_left);
    handle.get_context()->r_left = handle.get_context()->r_right;
    return result;
}

const std::string_view SocketFile::read_line() const
{
    size_t& r_left = this->handle.get_context()->r_left;
    size_t& r_right = this->handle.get_context()->r_right;
    if (r_right - r_left <= 1)
    {
        return std::string_view{""};
    }
    else
    {
        int fiannl = -1;
        for (size_t i = r_left; i <= r_right - 1; i++)
        {
            if (this->handle.get_context()->content[i] == '\r' &&
                this->handle.get_context()->content[i + 1] == '\n')
            {
                fiannl = i + 1;
                break;
            }
        }
        if (fiannl != -1)
        {
            auto tp = std::string_view(
                &this->handle.get_context()->content[this->handle.get_context()->r_left],
                fiannl - r_left + 1);
            r_left = fiannl + 1;
            return tp;
        }
        else
        {
            return std::string_view{""};
        }
    }
}

const std::string_view SocketFile::read_all() const
{
    return std::string_view(handle.get_context()->content.data(), handle.get_context()->r_right);
}

const void SocketFile::writeFile(std::string_view file)
{
    handle.context->waitingWrite = file;
    handle.context->w_left = 0;
    handle.context->w_right = file.length();
    return;
}

bool SocketFile::setNonBlocking()
{
    int flags = fcntl(this->handle.context->fd, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "获取socket标志失败: " << strerror(errno) << std::endl;
        return false;
    }
    if (fcntl(this->handle.context->fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "设置非阻塞模式失败: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

// LocalFiles实现
bool LocalFiles::add(std::string& path)
{
    auto [it, inserted] = fileMap.try_emplace(path, path);
    if (inserted)
    {
        return it->second.load(path);
    }
    return true; // 文件已存在
}

LocalFile& LocalFiles::get(std::string& path)
{
    return fileMap.at(path);
}

// SocketFiles实现
bool SocketFiles::add(int fd)
{
    auto [it, inserted] = fileMap.try_emplace(fd, SocketFile{fd});
    return inserted;
}

SocketFile& SocketFiles::get(int fd)
{
    return fileMap.at(fd);
}

const std::unordered_map<int, SocketFile>& SocketFiles::getMap()
{
    return fileMap;
}
int SocketFiles::eventGo()
{
    for (auto it = fileMap.begin(); it != fileMap.end();)
    {
        if (it->second.eventGo() == -1)
        {
            close(it->second.handle.context->fd);
            it = fileMap.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return 0;
}