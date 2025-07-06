#pragma once
#include "file.h"

enum class PortType : long long {};
enum class FileDescriptorType : long long {};

using port = PortType;
using fileDiscribe = FileDescriptorType;

class portForwarder : co_async
{
    int server_fd;           // 服务器套接字，用于监听
    port target_port;        // 目标端口
    std::string target_host; // 目标主机
    
    // 管理多个连接对
    struct ConnectionPair {
        SocketFile client;
        SocketFile target;
        bool active;
        
        ConnectionPair(int client_fd, int target_fd) 
            : client(client_fd), target(target_fd), active(true) {}
    };
    
    std::vector<std::unique_ptr<ConnectionPair>> connections;

  public:
    // 修改构造函数签名，明确参数含义
    portForwarder(port listen_port, port target_port, 
                  const std::string& target_host = "127.0.0.1",
                  const std::string& listen_host = "0.0.0.0");
    portForwarder(fileDiscribe from, fileDiscribe to);
    ~portForwarder();
    
    EventStatus eventGo() override;
    
  private:
    void acceptNewConnection();
    void forwardData();
    void cleanupDeadConnections();
};