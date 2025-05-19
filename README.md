# MyWebServer - 基于C++20协程的高性能Web服务器

## 项目概述

MyWebServer 是一个使用现代C++技术构建的高性能Web服务器，支持HTTP和WebSocket协议，并采用协程实现高并发处理。该项目实现了一个完整的聊天应用演示，包括用户认证、消息存储和实时通信功能。

## 特色与优点

### 技术特色

1. **现代C++协程技术**
   - 基于C++20协程特性实现非阻塞IO和高并发处理
   - 无需复杂的回调函数链，代码逻辑更加清晰直观
   - 协程调度器自动管理任务生命周期，降低内存泄漏风险

2. **多协议支持**
   - 完整实现HTTP/1.1协议，支持常见HTTP方法
   - 完整实现WebSocket协议，支持实时双向通信
   - 可扩展的协议注册机制，便于添加新协议支持

3. **跨平台兼容性**
   - 统一的平台抽象层，兼容Windows和Unix/Linux系统
   - 自动处理不同平台的网络API差异
   - 标准化错误处理机制

4. **高性能网络处理**
   - 非阻塞IO操作，提高并发性能
   - 智能缓冲区管理，高效处理大数据量传输
   - 采用协程实现的事件循环，减少线程切换开销

### 功能特色

1. **即时聊天应用示例**
   - 完整的用户注册和登录功能
   - 基于WebSocket的实时消息推送
   - 聊天历史记录持久化存储

2. **完善的会话管理**
   - 基于Cookie的会话跟踪
   - 会话超时自动处理
   - 安全的认证机制

3. **静态资源服务**
   - 自动识别MIME类型
   - 高效的文件缓存机制
   - 支持多种格式的静态文件

## 技术栈

- **语言**: C++20
- **数据库**: MySQL
- **构建系统**: CMake
- **依赖库**: OpenSSL (用于WebSocket安全握手)

## 架构设计

MyWebServer采用模块化设计，主要组件包括：

1. **协程管理器 (Co_Manager)**
   - 负责协程的创建、调度和销毁
   - 提供统一的事件循环管理

2. **协议处理层**
   - HttpServerUtil：处理HTTP请求解析和响应
   - WebSocketUtil：处理WebSocket帧解析和生成
   - 可扩展的协议注册机制

3. **网络抽象层**
   - SocketFile：封装底层socket操作
   - 平台无关的网络API

4. **数据持久化**
   - MySQL数据库接口
   - 用户数据和消息存储

## 使用示例

### 启动HTTP和WebSocket服务器

```cpp
#include "http.h"
#include "webSocketFile.h"

int main() {
    // 创建HTTP服务器实例
    HttpServer server;
    
    // 注册HTTP路由处理
    server.addCallbackFormat(
        Format{"/", Format::Type::same},
        [](serverFile& file) {
            file.write(HttpResponse::formLocalFile("index.html"));
        });
    
    // 注册WebSocket连接处理
    server.addCallbackFormat(
        Format{"/ws", Format::Type::same},
        [](serverFile& file) {
            if (WebSocketUtil::shouldbeUpdataToWS(file)) {
                file.write(WebSocketUtil::makeWebSocketHandshake(file));
                file.upgradeProtocol(Protocol::WebSocket);
            }
        });
    
    // 启动服务器
    Co_Start_Manager::getInstance().getManager().add(server);
    Co_Start_Manager::getInstance().start();
    
    return 0;
}
```

### 处理WebSocket消息

```cpp
// WebSocket消息处理
server.addCallbackFormat(
    Format{"/wsconnect", Format::Type::same},
    [](serverFile& socketfile) {
        if (socketfile.getAgreementType() == Protocol::WebSocket) {
            auto message = socketfile.getContent()["message"];
            if (!message.empty()) {
                // 处理接收到的消息
                std::cout << "收到消息: " << message << std::endl;
                
                // 发送响应
                socketfile.write(std::string(
                    WebSocketResponse::text("服务器已接收您的消息")
                ));
            }
        }
    });
```

## 性能优势

1. **低内存占用**
   - 协程相比线程有更低的内存占用
   - 高效的缓冲区管理减少内存复制

2. **高并发处理能力**
   - 非阻塞IO结合协程实现真正的异步处理
   - 能够处理数千级别的并发连接

3. **快速响应**
   - 事件驱动模型确保请求快速得到处理
   - 协程调度器优化任务执行顺序

## 未来发展方向

1. 实现HTTPS和WSS (WebSocket Secure) 支持
2. 添加HTTP/2协议支持
3. 增强数据库连接池管理
4. 实现更完善的路由系统

---

*MyWebServer 是一个不断发展的项目，欢迎贡献代码和提出建议。*