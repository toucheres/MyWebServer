#include <errno.h>
#include <file.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// LocalFile实现
LocalFile::LocalFile(std::string a_path) : path(std::move(a_path)), size(0)
{
    // 构造函数初始化路径，不加载内容
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
    handle.get_context()->left = 0;
    handle.get_context()->right = 0;
    handle.get_context()->content.resize(4096); // 预分配缓冲区
    return true;
}

Task<> SocketFile::eventfun(std::shared_ptr<CONTEXT> context)
{
    while (true)
    {
        // 检查缓冲区是否需要扩容
        if (context->right >= context->content.size())
        {
            context->content.resize(context->content.size() * 2);
        }

        // 异步读取数据
        ssize_t n = read(context->fd, context->content.data() + context->right,
                         context->content.size() - context->right);

        if (n > 0)
        {
            context->right += n;
        }
        else if (n == 0)
        {
            // EOF 文件结束
            co_yield {};
        }
        else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            // 暂无数据可读，暂停协程
            co_yield {};
        }
        else
        {
            // 读取错误
            std::cerr << "Socket read error: " << strerror(errno) << "fd: " << context->fd
                      << std::endl;
        }

        co_yield {};
    }
}

int SocketFile::eventGo() const
{
    handle.resume();
    return 0;
}

const std::string_view SocketFile::read_added() const
{
    if (handle.get_context()->left >= handle.get_context()->right)
    {
        return std::string_view();
    }

    std::string_view result(handle.get_context()->content.data() + handle.get_context()->left,
                            handle.get_context()->right - handle.get_context()->left);
    handle.get_context()->left = handle.get_context()->right;
    return result;
}

const std::string_view SocketFile::read_all() const
{
    return std::string_view(handle.get_context()->content.data(), handle.get_context()->right);
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
int SocketFiles::eventGo() const
{
    for (auto& each : fileMap)
    {
        each.second.eventGo();
    }
    return 0;
}