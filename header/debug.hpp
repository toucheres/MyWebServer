#pragma once
#include "corutine.hpp"
#include "file.h"
#include "format.h"
#include "forworder.h"
#include "http.h"
#include "httpClient.h"
#include "httpServerFile.h"
#include "json.hpp"
#include "platform.h"
#include "serverFile.h"
#include "threadpool.hpp"
#include "webSocketFile.h"
#include <chrono>
#include <cstdlib>
#include <string>
#include <thread>
namespace Debugger
{
    class print
    {
      public:
    };
    namespace network
    {
        inline auto TcpTest()
        {
            int so = HttpServerUtil::makeSocket();
            HttpServerUtil::bindSocket(so, 8080);
            HttpServerUtil::listenSocket(so);
            std::cout << "TCP测试服务器启动在端口 8080" << std::endl;

            while (1)
            {
                sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                int conn = HttpServerUtil::acceptSocket(so, (sockaddr*)&client_addr, &addr_len);

                if (conn < 0)
                {
                    std::cerr << "接受连接失败" << std::endl;
                    continue;
                }

                // 获取客户端IP地址
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                std::cout << "客户端连接: " << client_ip << ":" << ntohs(client_addr.sin_port)
                          << std::endl;

                // 设置为非阻塞模式
                platform::setNonBlocking(conn);

                // 处理客户端数据
                while (1)
                {
                    char tpdata[1024];
                    memset(tpdata, 0, sizeof(tpdata));

                    int bytes_read = platform::readSocket(conn, tpdata, sizeof(tpdata) - 1);

                    if (bytes_read > 0)
                    {
                        // 成功读取数据
                        tpdata[bytes_read] = '\0';
                        std::cout << "接收到数据 [" << bytes_read << " 字节]: " << tpdata
                                  << std::endl;

                        // 回显数据给客户端
                        std::string response = "Echo: " + std::string(tpdata);
                        int bytes_sent =
                            platform::writeSocket(conn, response.c_str(), response.length());

                        if (bytes_sent > 0)
                        {
                            std::cout << "回发数据 [" << bytes_sent << " 字节]: " << response
                                      << std::endl;
                        }
                        else
                        {
                            std::cerr << "发送数据失败" << std::endl;
                            break;
                        }
                    }
                    else if (bytes_read == 0)
                    {
                        // 客户端关闭连接
                        std::cout << "客户端 " << client_ip << " 断开连接" << std::endl;
                        break;
                    }
                    else if (bytes_read == -1)
                    {
                        // 检查错误类型
                        int error = platform::getLastError();
                        if (platform::isWouldBlock(error))
                        {
                            // 非阻塞模式下暂无数据，继续等待
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                            continue;
                        }
                        else if (platform::isConnectionReset(error))
                        {
                            std::cout << "客户端 " << client_ip << " 重置连接" << std::endl;
                            break;
                        }
                        else
                        {
                            std::cerr << "读取数据错误: " << platform::getErrorString(error)
                                      << std::endl;
                            break;
                        }
                    }
                }

                // 关闭客户端连接
                platform::closeSocket(conn);
                std::cout << "关闭与客户端 " << client_ip << " 的连接" << std::endl;
            }
        }
    } // namespace network
} // namespace Debugger