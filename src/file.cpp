#include <file.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

// LocalFile实现
LocalFile::LocalFile(std::string a_path) : path(std::move(a_path)), size(0) {
    // 构造函数初始化路径，不加载内容
}

LocalFile::LocalFile(LocalFile&& move) 
    : content(std::move(move.content)), 
      path(std::move(move.path)), 
      size(move.size),
      test(move.test) {
    move.size = 0;
}

bool LocalFile::load(std::string& path) {
    this->path = path;
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }

    size = file.tellg();
    file.seekg(0, std::ios::beg);

    content.resize(size);
    if (size > 0) {
        file.read(content.data(), size);
    }
    
    test = std::string_view(content.data(), size);
    return true;
}

std::string_view LocalFile::read() {
    return test;
}

// SocketFile实现
SocketFile::SocketFile(int a_fd) {
    load(a_fd);
}

SocketFile::SocketFile(SocketFile&& move) 
    : read_async_handle(std::move(move.read_async_handle)) {
}

// SocketFile::SocketFile(const SocketFile& copy) 
//     : read_async_handle(copy.read_async_handle) {
// }

bool SocketFile::load(int a_fd) {
    read_async_handle.get_context().fd = a_fd;
    read_async_handle.get_context().left = 0;
    read_async_handle.get_context().right = 0;
    read_async_handle.get_context().content.resize(4096); // 预分配缓冲区
    return true;
}

Task<> SocketFile::eventfun(CONTEXT& context) {
    while (true) {
        // 检查缓冲区是否需要扩容
        if (context.right >= context.content.size()) {
            context.content.resize(context.content.size() * 2);
        }

        // 异步读取数据
        ssize_t n = read(context.fd, context.content.data() + context.right, 
                         context.content.size() - context.right);

        if (n > 0) {
            context.right += n;
        }
        else if (n == 0) {
            // EOF 文件结束
            co_yield {};
        }
        else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            // 暂无数据可读，暂停协程
            co_yield {};
        }
        else {
            // 读取错误
            std::cerr << "Socket read error: " << strerror(errno) << std::endl;
        }

        co_yield {};
    }
}

int SocketFile::eventGo() {
    read_async_handle.resume();
    return 0;
}

std::string_view SocketFile::read_added() {
    if (read_async_handle.get_context().left >= read_async_handle.get_context().right) {
        return std::string_view();
    }
    
    std::string_view result(read_async_handle.get_context().content.data() + read_async_handle.get_context().left,
                           read_async_handle.get_context().right - read_async_handle.get_context().left);
    read_async_handle.get_context().left = read_async_handle.get_context().right;
    return result;
}

std::string_view SocketFile::read_all() {
    return std::string_view(read_async_handle.get_context().content.data(),
                           read_async_handle.get_context().right);
}

// LocalFiles实现
bool LocalFiles::add(std::string& path) {
    auto [it, inserted] = fileMap.try_emplace(path, path);
    if (inserted) {
        return it->second.load(path);
    }
    return true; // 文件已存在
}

LocalFile& LocalFiles::get(std::string& path) {
    return fileMap.at(path);
}

// SocketFiles实现
bool SocketFiles::add(int fd) {
    std::string key = std::to_string(fd);
    auto [it, inserted] = fileMap.try_emplace(key, fd);
    return inserted;
}

SocketFile& SocketFiles::get(int fd) {
    std::string key = std::to_string(fd);
    return fileMap.at(key);
}
