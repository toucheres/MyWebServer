#include "corutine.hpp"
#include "platform.h" // 添加平台兼容性头文件
#include <errno.h>
#include <file.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string.h>
#include <string_view>
#include <unistd.h>

// LocalFile实现
LocalFile::LocalFile(std::string a_path) : path(std::move(a_path)), size(0)
{
    load(path);
}

LocalFile::LocalFile(LocalFile&& move)
    : content(std::move(move.content)), path(std::move(move.path)), size(move.size),
      fileview(move.fileview)
{
    move.size = 0;
}

bool LocalFile::load(std::string& path)
{
    this->path = path;
    std::ifstream file(path.data(), std::ios::binary);
    if (!file.is_open())
    {
        return false;
        content.clear();
        fileview = "";
    }

    file.seekg(0, std::ios::end);
    std::streamsize size_file = file.tellg();
    size = size_file;
    file.seekg(0, std::ios::beg);

    content.resize(size);
    if (size > 0)
    {
        file.read(content.data(), size);
    }

    fileview = std::string_view(content.data(), size);
    return true;
}

std::string_view LocalFile::read()
{
    return fileview;
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
void SocketFile::closeIt()
{
    handle.get_context()->fd_state = WRONG;
    this->eventGo();
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
        
        // 设置非阻塞模式 - 使用平台无关的函数
        platform::setNonBlocking(context->fd);
        
        // 读取数据 - 使用平台无关的接口
        ssize_t n = platform::readSocket(context->fd, 
                                        context->content.data() + context->r_right,
                                        context->content.size() - context->r_right);
        
        if (n > 0)
        {
            context->r_right += n;
            continue;
        }
        else if (n == 0)
        {
            context->fd_state = WRONG;
            break;
        }
        else if (n < 0)
        {
            // 使用平台无关的错误检查
            int lastError = platform::getLastError();
            if (platform::isWouldBlock(lastError))
            {
                // 暂无数据可读，暂停协程
            }
            else
            {
                // 读取错误 - 统一使用平台无关函数
                if (platform::isConnectionReset(lastError))
                {
                    std::cerr << "客户端断开连接: " 
                              << platform::getErrorString(lastError)
                              << ", fd: " << context->fd << std::endl;
                }
                else
                {
                    std::cerr << "Socket read error: " 
                              << platform::getErrorString(lastError)
                              << ", fd: " << context->fd << std::endl;
                }
                context->fd_state = WRONG;
                co_yield {};
            }
        }
        
        // 写数据
        if (!context->waitingWrites.empty() &&
            (context->waitingWrites.front().w_right > context->waitingWrites.front().w_left))
        {
            ssize_t written = platform::writeSocket(
                context->fd,
                &context->waitingWrites.front().waitingWrite[context->waitingWrites.front().w_left],
                context->waitingWrites.front().w_right - context->waitingWrites.front().w_left);
            
            if (written == SOCKET_ERROR_RETURN)
            {
                // 处理写入错误 - 跨平台处理
                int lastError = platform::getLastError();
                
                // 特定错误处理
                if (lastError == EINTR) // EINTR是一个跨平台的宏
                {
                    // 被信号中断，继续尝试
                    continue;
                }
                else if (platform::isWouldBlock(lastError))
                {
                    // 非阻塞模式下缓冲区已满，可以稍后重试
                }
                else
                {
                    // 其他错误（连接关闭等）
                    std::cerr << "写入失败: " << platform::getErrorString(lastError) << std::endl;
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
                context->waitingWrites.front().w_left += written;
                if (context->waitingWrites.front().w_left == context->waitingWrites.front().w_right)
                {
                    context->waitingWrites.pop();
                }
                continue;
            }
        }
        co_yield {};
    }
    co_return;
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

const std::string_view SocketFile::read_num(size_t num) const
{
    if (handle.get_context()->r_left - handle.get_context()->r_right >= num)
    {
        auto tp = std::string_view{
            handle.get_context()->content.data() + handle.get_context()->r_left, num};
        handle.get_context()->r_left += num;
        return tp;
    }
    return "";
}

const std::string_view SocketFile::read_line() const
{
    return read_until("\r\n");
}

const std::string_view SocketFile::read_all() const
{
    return std::string_view(handle.get_context()->content.data(), handle.get_context()->r_right);
}

const std::string_view SocketFile::read_until(const std::string_view delimiter) const
{
    //[bug]有问题，像是pg导致的
    size_t& r_left = this->handle.get_context()->r_left;
    size_t& r_right = this->handle.get_context()->r_right;

    // 确保有足够的数据可读
    if (r_right - r_left < delimiter.length())
    {
        return std::string_view{""};
    }
    else
    {
        size_t found_pos = std::string::npos;

        // 在缓冲区中查找指定的结束符
        for (size_t i = r_left; i <= r_right - delimiter.length(); i++)
        {
            bool match = true;
            for (size_t j = 0; j < delimiter.length(); j++)
            {
                if (this->handle.get_context()->content[i + j] != delimiter[j])
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                found_pos = i;
                break;
            }
        }

        if (found_pos != std::string::npos)
        {
            // 计算返回的字符串长度（包括结束符）
            size_t result_length = found_pos - r_left + delimiter.length();

            // 创建包含结束符的字符串视图
            auto result =
                std::string_view(&this->handle.get_context()->content[r_left], result_length);

            // 更新读取位置
            r_left = found_pos + delimiter.length();

            return result;
        }
        else
        {
            return std::string_view{""};
        }
    }
}

const void SocketFile::writeFile(const std::string file)
{
    return handle.context->waitingWrites.push(file);
}

bool SocketFile::setNonBlocking()
{
    return platform::setNonBlocking(this->handle.context->fd);
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

LocalFile& LocalFiles::get(const std::string& path)
{
    auto it = fileMap.find(path);
    if (it != fileMap.end())
    {
        return it->second;
    }
    auto [newIt, _] = fileMap.emplace(path, path);
    return newIt->second;
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
            platform::closeSocket(it->second.handle.context->fd);
            it = fileMap.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return 0;
}
SocketFile::CONTEXT::writingFile::writingFile(std::string&& move)
{
    this->waitingWrite = std::move(move);
    w_right = waitingWrite.length();
}
SocketFile::CONTEXT::writingFile::writingFile(const std::string& copy)
{
    this->waitingWrite = copy;
    w_right = waitingWrite.length();
}

int async_in_out::eventGo()
{
    in.eventGo();
    out.eventGo();
    // std::cout << "in now:" << in.read_all();
    return 0;
}
async_in_out::async_in_out()
{
    Co_Start_Manager::getInstance().manager.add(this);
}
std::string_view async_in_out::read_added()
{
    return in.read_added();
}
std::string_view async_in_out::read_all()
{
    return in.read_all();
}
std::string_view async_in_out::read_until()
{
    return in.read_until("\n");
}
std::string_view async_in_out::read_line()
{
    return in.read_until("\n");
}
void async_in_out::writeFile(std::string file)
{
    return out.writeFile(file);
}

async_in_out& async_in_out::getInstance()
{
    static async_in_out instance;
    return instance;
}
