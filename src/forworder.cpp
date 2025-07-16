#include "forworder.h"
#include "corutine.hpp"
#include "format.h"
#include "httpServerFile.h"
#include <algorithm>
#include "httpMessage.h"
// tcpForwarder::tcpForwarder(port a_from, port a_to, const std::string& target_host)
//     : target_port(a_to), target_host(target_host)
// {
//     // 创建并配置服务器套接字
//     server_fd = HttpServerUtil::makeSocket();
//     if (server_fd == -1)
//     {
//         throw std::runtime_error("Failed to create server socket");
//     }

//     // 设置端口复用
//     if (!HttpServerUtil::setReuseAddr(server_fd))
//     {
//         platform::closeSocket(server_fd);
//         throw std::runtime_error("Failed to set SO_REUSEADDR");
//     }

//     // 设置为非阻塞模式
//     if (!platform::setNonBlocking(server_fd))
//     {
//         platform::closeSocket(server_fd);
//         throw std::runtime_error("Failed to set non-blocking mode");
//     }

//     // 绑定到源端口
//     if (HttpServerUtil::bindSocket(server_fd, static_cast<long long>(a_from), "0.0.0.0") == -1)
//     {
//         platform::closeSocket(server_fd);
//         throw std::runtime_error("Failed to bind to port " +
//                                  std::to_string(static_cast<long long>(a_from)));
//     }

//     // 开始监听
//     if (!HttpServerUtil::listenSocket(server_fd, 10))
//     {
//         platform::closeSocket(server_fd);
//         throw std::runtime_error("Failed to listen on port " +
//                                  std::to_string(static_cast<long long>(a_from)));
//     }

//     // std::cout << "Port forwarder listening on port " << static_cast<long long>(a_from) << " ->
//     "
//     //       << target_host << ":" << static_cast<long long>(a_to) << std::endl;
// }

tcpForwarder::~tcpForwarder()
{
    if (server_fd != -1)
    {
        platform::closeSocket(server_fd);
    }

    // 清理所有连接
    connections.clear();
}

EventStatus tcpForwarder::eventGo()
{
    // 1. 尝试接受新连接
    acceptNewConnection();

    // 2. 处理所有现有连接的数据转发
    forwardData();

    // 3. 清理已断开的连接
    cleanupDeadConnections();

    return EventStatus::Continue;
}

void tcpForwarder::acceptNewConnection()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 尝试接受新连接（非阻塞）
    int client_fd =
        HttpServerUtil::acceptSocket(server_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd == -1)
    {
        // 没有新连接或出错，这在非阻塞模式下是正常的
        return;
    }

    if (client_fd == -2)
    {
        // 特殊返回码：没有待处理的连接
        return;
    }

    // 设置客户端套接字为非阻塞
    if (!platform::setNonBlocking(client_fd))
    {
        platform::closeSocket(client_fd);
        std::cerr << "Failed to set client socket non-blocking" << std::endl;
        return;
    }

    // 连接到目标服务器
    int target_fd =
        HttpServerUtil::connectToServer(target_host, static_cast<long long>(target_port));
    if (target_fd == -1)
    {
        platform::closeSocket(client_fd);
        std::cerr << "Failed to connect to target " << target_host << ":"
                  << static_cast<long long>(target_port) << std::endl;
        return;
    }

    // 设置目标套接字为非阻塞
    if (!platform::setNonBlocking(target_fd))
    {
        platform::closeSocket(client_fd);
        platform::closeSocket(target_fd);
        std::cerr << "Failed to set target socket non-blocking" << std::endl;
        return;
    }

    // 创建新的连接对
    auto connection = std::make_unique<ConnectionPair>(client_fd, target_fd);
    connections.push_back(std::move(connection));

    // 获取客户端IP地址用于日志
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    // std::cout << "New connection from " << client_ip << ":" << ntohs(client_addr.sin_port) << "
    // -> "
    //       << target_host << ":" << static_cast<long long>(target_port)
    //     << " (Total: " << connections.size() << " connections)" << std::endl;
}

void tcpForwarder::forwardData()
{
    for (auto& conn : connections)
    {
        if (!conn->active)
            continue;

        bool client_active = true;
        bool target_active = true;

        // 从客户端读取数据，转发到目标服务器
        try
        {
            std::string client_data = std::string(conn->client.read_added());
            if (!client_data.empty())
            {
                conn->target.writeFile(client_data);
                // 可选：记录转发的数据量
                // std::cout << "Client->Target: " << client_data.size() << " bytes" << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error reading from client: " << e.what() << std::endl;
            client_active = false;
        }

        // 从目标服务器读取数据，转发到客户端
        try
        {
            std::string target_data = std::string(conn->target.read_added());
            if (!target_data.empty())
            {
                conn->client.writeFile(target_data);
                // 可选：记录转发的数据量
                // std::cout << "Target->Client: " << target_data.size() << " bytes" << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error reading from target: " << e.what() << std::endl;
            target_active = false;
        }

        // 检查连接状态
        if (!client_active || !target_active || conn->client.eventGo() == EventStatus::Stop ||
            conn->target.eventGo() == EventStatus::Stop)
        {
            conn->active = false;
            // std::cout << "Connection closed" << std::endl;
        }
    }
}

void tcpForwarder::cleanupDeadConnections()
{
    // 移除已断开的连接
    auto it =
        std::remove_if(connections.begin(), connections.end(),
                       [](const std::unique_ptr<ConnectionPair>& conn) { return !conn->active; });

    if (it != connections.end())
    {
        size_t removed = std::distance(it, connections.end());
        connections.erase(it, connections.end());
        // std::cout << "Cleaned up " << removed << " dead connections. "
        //       << "Active connections: " << connections.size() << std::endl;
    }
}

// 文件描述符版本的构造函数
tcpForwarder::tcpForwarder(fileDiscribe from, fileDiscribe to)
{
    // 这个版本直接使用已有的文件描述符
    // 创建单个连接对
    auto connection =
        std::make_unique<ConnectionPair>(static_cast<int>(from), static_cast<int>(to));
    connections.push_back(std::move(connection));

    server_fd = -1; // 不需要监听套接字
    // std::cout << "Direct file descriptor forwarding initialized" << std::endl;
}

// 新增构造函数：支持监听主机和目标主机
tcpForwarder::tcpForwarder(port listen_port, port target_port, const std::string& target_host,
                           const std::string& listen_host)
    : target_port(target_port), target_host(target_host)
{
    // 创建并配置服务器套接字
    server_fd = HttpServerUtil::makeSocket();
    if (server_fd == -1)
    {
        throw std::runtime_error("Failed to create server socket");
    }

    // 设置端口复用
    if (!HttpServerUtil::setReuseAddr(server_fd))
    {
        platform::closeSocket(server_fd);
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }

    // 设置为非阻塞模式
    if (!platform::setNonBlocking(server_fd))
    {
        platform::closeSocket(server_fd);
        throw std::runtime_error("Failed to set non-blocking mode");
    }

    // 绑定到监听端口和地址
    if (HttpServerUtil::bindSocket(server_fd, static_cast<long long>(listen_port), listen_host) ==
        -1)
    {
        platform::closeSocket(server_fd);
        throw std::runtime_error("Failed to bind to " + listen_host + ":" +
                                 std::to_string(static_cast<long long>(listen_port)));
    }

    // 开始监听
    if (!HttpServerUtil::listenSocket(server_fd, 10))
    {
        platform::closeSocket(server_fd);
        throw std::runtime_error("Failed to listen on port " +
                                 std::to_string(static_cast<long long>(listen_port)));
    }

    // std::cout << "Port forwarder listening on " << listen_host << ":"
    //           << static_cast<long long>(listen_port) << " -> " << target_host << ":"
    //           << static_cast<long long>(target_port) << std::endl;
}
httpForwarder::httpForwarder(port listen_port, port target_port, const std::string& target_host,
                             const std::string& listen_host)
    : forwardserver((long long)listen_port, listen_host)
{
    forwardserver.addCallbackFormat(
        Format{"", Format::Type::prefix},
        [](serverFile& in)
        {
            std::string get = in.getContent()[HttpRequst::CilentKey::orignal_content];
            
        });
}
EventStatus httpForwarder::eventGo()
{
    this->ma.go();
    return EventStatus::Continue;
}