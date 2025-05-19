# MyWebServer 操作文档

## 目录

1. [简介](#1-简介)
2. [安装与配置](#2-安装与配置)
3. [服务器启动与管理](#3-服务器启动与管理)
4. [功能使用指南](#4-功能使用指南)
5. [开发指南](#5-开发指南)
6. [API参考](#6-api参考)
7. [常见问题与故障排除](#7-常见问题与故障排除)
8. [附录](#8-附录)

## 1. 简介

### 1.1 项目概述

MyWebServer 是一个基于 C++20 协程技术构建的高性能 Web 服务器，支持 HTTP 和 WebSocket 协议，实现了非阻塞 IO 和高并发处理。该项目包含一个完整的聊天应用演示，涵盖用户认证、消息存储和实时通信功能。

### 1.2 主要特性

- 基于 C++20 协程实现的高性能网络处理
- 多协议支持（HTTP/1.1、WebSocket）
- 跨平台兼容（Windows、Linux、macOS）
- 聊天应用示例，支持用户登录和消息持久化
- 基于 MySQL 的数据存储
- 静态文件服务

### 1.3 系统要求

- C++20 兼容的编译器（GCC 10+、Clang 10+、MSVC 2019+）
- CMake 3.16 或更高版本
- MySQL 5.7 或更高版本
- OpenSSL 开发库

## 2. 安装与配置

### 2.1 获取源码

```bash
git clone https://github.com/yourusername/MyWebServer.git
cd MyWebServer
```

### 2.2 编译构建

#### 2.2.1 Linux/macOS

```bash
mkdir build && cd build
cmake ..
make
```

#### 2.2.2 Windows

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### 2.3 配置 MySQL 数据库

1. 创建数据库和用户

```sql
CREATE DATABASE chat_db;
CREATE USER 'webserver'@'localhost' IDENTIFIED BY 'WebServer@2025';
GRANT ALL PRIVILEGES ON chat_db.* TO 'webserver'@'localhost';
FLUSH PRIVILEGES;
```

2. 初始化表结构（服务器首次启动会自动创建表结构，也可手动执行以下 SQL）

```sql
USE chat_db;

-- 用户表
CREATE TABLE IF NOT EXISTS users (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(50) NOT NULL UNIQUE,
  password VARCHAR(100) NOT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 消息表
CREATE TABLE IF NOT EXISTS messages (
  id INT AUTO_INCREMENT PRIMARY KEY,
  sender VARCHAR(50) NOT NULL,
  content TEXT NOT NULL,
  timestamp BIGINT NOT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### 2.4 静态文件准备

确保您的工作目录中有必要的静态文件：

- `index.html` - 主页面
- `404.html` - 错误页面（可选）
- 其他静态资源（CSS、JavaScript、图片等）

## 3. 服务器启动与管理

### 3.1 启动服务器

```bash
# 从构建目录运行
cd build
./MyWebServer
```

服务器默认在 8080 端口启动。启动信息会显示在控制台，包括：

- 服务器启动状态
- 数据库连接状态
- 已加载的静态资源
- 注册的路由信息

### 3.2 服务器配置

服务器的配置可以通过 `main.cpp` 中修改代码来调整。关键配置包括：

- 监听端口
- 数据库连接信息
- 协程调度间隔

示例配置修改：

```cpp
// 修改监听端口
HttpServer server("0.0.0.0", 9090);

// 修改数据库连接
mysqldb.connect("localhost", "customuser", "password", "custom_db", 3306);

// 修改协程调度间隔为10毫秒
coManagerInstance.setLoopInterval(std::chrono::milliseconds(10));
```

### 3.3 停止服务器

在控制台按 `Ctrl+C` 停止服务器。

## 4. 功能使用指南

### 4.1 访问Web界面

在浏览器中访问 `http://localhost:8080/` 可以打开聊天应用界面。

### 4.2 用户管理

#### 4.2.1 注册新用户

访问 `http://localhost:8080/register/{username}/{password}` 进行注册。

要求：
- 用户名至少3个字符
- 密码至少6个字符

#### 4.2.2 用户登录

访问 `http://localhost:8080/login/{username}/{password}` 进行登录。

登录成功后服务器会设置会话Cookie，有效期为1小时。

#### 4.2.3 检查会话状态

访问 `http://localhost:8080/checkSession` 可检查当前登录状态。

### 4.3 聊天功能

#### 4.3.1 WebSocket连接

前端JavaScript连接示例：

```javascript
const ws = new WebSocket("ws://localhost:8080/wsconnect/chat");

ws.onopen = function() {
  console.log("WebSocket连接已建立");
};

ws.onmessage = function(evt) {
  const message = evt.data;
  console.log("收到消息: " + message);
  // 处理消息...
};
```

#### 4.3.2 发送消息

消息格式：

```
sender:{用户名}
content:{消息内容}
time:{可选时间戳}
```

如：

```javascript
ws.send("sender:张三\ncontent:你好，大家好！\ntime:2023-06-01 12:30:45");
```

#### 4.3.3 查看历史消息

访问 `http://localhost:8080/getHistory` 可获取最近100条消息。

## 5. 开发指南

### 5.1 项目结构

```
MyWebServer/
├── header/              # 头文件
│   ├── corutine.hpp     # 协程实现
│   ├── file.h           # 文件和socket处理
│   ├── format.h         # 路由格式解析
│   ├── http.h           # HTTP服务器
│   ├── httpServerFile.h # HTTP协议处理
│   ├── platform.h       # 跨平台兼容
│   ├── protocol_constants.h # 协议常量
│   ├── serverFile.h     # 服务器文件抽象
│   └── webSocketFile.h  # WebSocket协议处理
├── src/                 # 源文件
│   ├── file.cpp         # 文件操作实现
│   ├── format.cpp       # 格式解析实现
│   ├── http.cpp         # HTTP服务器实现
│   ├── httpServerFile.cpp # HTTP协议处理实现
│   ├── main.cpp         # 主程序入口
│   ├── serverFile.cpp   # 服务器文件实现
│   └── webSocketFile.cpp # WebSocket协议实现
└── CMakeLists.txt       # CMake构建文件
```

### 5.2 添加新的HTTP路由

在 `main.cpp` 中添加新的回调格式：

```cpp
httpServer.addCallbackFormat(
    Format{"/custom-path", Format::Type::same},
    [](serverFile& socketfile) {
        if (socketfile.getAgreementType() == Protocol::HTTP) {
            // 处理HTTP请求
            HttpResponse response{200};
            response.with_content("自定义响应内容");
            socketfile.write(response);
        }
        return;
    });
```

### 5.3 使用路径参数

使用 `scanf` 风格的格式字符串可以捕获URL参数：

```cpp
// 匹配路径如 /users/123/profile
httpServer.addCallbackFormat(
    Format{"/users/%d/profile", Format::Type::scanf},
    [](serverFile& socketfile) {
        auto res = Format{"/users/%d/profile", Format::Type::scanf}.parse(
            socketfile.getContent()["path"]);
        
        if (res) {
            int userId = std::get<int>((*res)[0]);
            std::cout << "User ID: " << userId << std::endl;
            
            // 处理请求...
            socketfile.write(HttpResponse{200}.with_content("用户资料"));
        } else {
            socketfile.write(HttpResponse{400});
        }
    });
```

### 5.4 处理WebSocket连接

```cpp
httpServer.addCallbackFormat(
    Format{"/ws-endpoint", Format::Type::same},
    [](serverFile& socketfile) {
        // 处理协议升级
        if (socketfile.getAgreementType() == Protocol::HTTP) {
            if (WebSocketUtil::shouldbeUpdataToWS(socketfile)) {
                socketfile.write(WebSocketUtil::makeWebSocketHandshake(socketfile));
                socketfile.upgradeProtocol(Protocol::WebSocket);
                return;
            }
        }
        // 处理WebSocket消息
        else if (socketfile.getAgreementType() == Protocol::WebSocket) {
            auto message = socketfile.getContent()["message"];
            if (!message.empty()) {
                // 处理接收到的消息
                std::cout << "接收到WebSocket消息: " << message << std::endl;
                
                // 发送响应
                socketfile.write(std::string(
                    WebSocketResponse::text("服务器响应")
                ));
            }
        }
    });
```

### 5.5 数据库操作

```cpp
// 查询示例
auto res = mysqldb.query("SELECT * FROM users WHERE username = 'admin'");
if (res) {
    auto row = mysqldb.fetchRow();
    while (!row.empty()) {
        std::cout << "用户ID: " << row[0] << ", 用户名: " << row[1] << std::endl;
        row = mysqldb.fetchRow();
    }
}

// 插入示例
std::string username = "newuser";
std::string password = "password123";
mysqldb.query("INSERT INTO users (username, password) VALUES ('" + 
              username + "', '" + password + "')");
```

## 6. API参考

### 6.1 HTTP服务器API

#### HttpServer 类

```cpp
// 创建HTTP服务器
HttpServer server("0.0.0.0", 8080);

// 添加路由回调
server.addCallbackFormat(Format{路径格式}, 回调函数);

// 移除路由回调
server.removeCallbackFormat(Format{路径格式});

// 获取已连接的客户端
const auto& clients = server.getfilemap();
```

#### HttpResponse 类

```cpp
// 创建HTTP响应
HttpResponse response{状态码};

// 添加响应头
response.addHeader("Content-Type", "application/json");

// 设置响应内容
response.with_content("{\"status\":\"success\"}", "application/json");

// 从文件创建响应
HttpResponse fileResponse = HttpResponse::formLocalFile("index.html");
```

### 6.2 WebSocket API

#### WebSocketUtil 类

```cpp
// 检查是否应升级到WebSocket
bool shouldUpgrade = WebSocketUtil::shouldbeUpdataToWS(httpFile);

// 创建握手响应
std::string handshake = WebSocketUtil::makeWebSocketHandshake(httpFile);

// 解析WebSocket帧
std::string payload = WebSocketUtil::parseWebSocketFrame(frame);
```

#### WebSocketResponse 类

```cpp
// 创建文本消息
WebSocketResponse textMsg = WebSocketResponse::text("文本消息");

// 创建二进制消息
WebSocketResponse binaryMsg = WebSocketResponse::binary(binaryData);

// 创建控制帧
WebSocketResponse pingMsg = WebSocketResponse::ping();
WebSocketResponse pongMsg = WebSocketResponse::pong();
WebSocketResponse closeMsg = WebSocketResponse::close(1000, "关闭原因");
```

### 6.3 服务器文件API

```cpp
// 获取协议类型
Protocol protocol = file.getAgreementType();

// 获取请求内容
auto& headers = file.getContent();
std::string path = headers["path"];
std::string method = headers["method"];

// 写入响应
file.write(response);

// 升级协议
file.upgradeProtocol(Protocol::WebSocket);

// 设置回调
file.setCallback([](serverFile& file) {
    // 处理请求
});
```

### 6.4 协程管理API

```cpp
// 获取协程管理器实例
auto& coManager = Co_Start_Manager::getInstance();

// 添加任务
coManager.getManager().add(server);

// 设置调度间隔
coManager.setLoopInterval(std::chrono::milliseconds(1));

// 启动协程调度
coManager.start();
```

## 7. 常见问题与故障排除

### 7.1 服务器无法启动

**问题**: 服务器启动时报错或立即退出。

**解决方案**:
1. 检查端口是否被占用：`netstat -tuln | grep 8080`
2. 检查数据库连接配置是否正确
3. 确保有足够的权限运行服务器
4. 查看控制台错误信息

### 7.2 数据库连接失败

**问题**: 数据库连接失败消息："数据库连接失败: ..."

**解决方案**:
1. 确认MySQL服务正在运行
2. 验证用户名和密码是否正确
3. 检查数据库名称是否存在
4. 确认主机地址和端口配置

### 7.3 WebSocket连接问题

**问题**: 无法建立WebSocket连接。

**解决方案**:
1. 检查WebSocket URL是否正确
2. 确保URL格式为 `ws://hostname:port/wsconnect/xxx`
3. 检查浏览器控制台网络请求，确认握手请求
4. 服务端日志查看握手响应是否正确

### 7.4 静态文件404错误

**问题**: 访问静态文件返回404。

**解决方案**:
1. 确保文件存在于正确的路径
2. 检查服务器工作目录
3. 检查文件权限

## 8. 附录

### 8.1 性能优化建议

1. **协程调度优化**
   - 根据服务器负载调整协程调度间隔
   - 对于高负载服务器，建议使用更短的调度间隔

2. **数据库连接优化**
   - 实现连接池以重用数据库连接
   - 使用预处理语句提高SQL执行效率

3. **静态资源优化**
   - 启用静态资源缓存
   - 考虑使用压缩传输静态资源

### 8.2 安全建议

1. **预防SQL注入**
   - 使用参数化查询
   - 对用户输入进行严格验证

2. **认证与授权**
   - 实现更强的密码策略
   - 考虑添加CSRF保护

3. **WebSocket安全**
   - 实现WSS (WebSocket Secure)
   - 验证WebSocket消息来源

### 8.3 框架扩展思路

1. **添加HTTPS支持**
   - 实现SSL/TLS握手
   - 管理证书

2. **添加更多协议支持**
   - HTTP/2
   - MQTT

3. **添加插件系统**
   - 实现中间件机制
   - 支持动态加载模块

---

## 附录A: 示例代码

### 完整服务器示例

```cpp
#include "http.h"
#include "httpServerFile.h"
#include "webSocketFile.h"
#include <iostream>

int main() {
    // 创建HTTP服务器
    HttpServer server;
    
    // 配置数据库连接
    MySQLHandle db;
    if (!db.connect("localhost", "webserver", "WebServer@2025", "chat_db", 3306)) {
        std::cerr << "数据库连接失败: " << db.getLastError() << std::endl;
        return 1;
    }
    
    // 静态文件服务
    server.addCallbackFormat(
        Format{"/", Format::Type::same},
        [](serverFile& file) {
            file.write(HttpResponse::formLocalFile("index.html"));
        });
    
    // 添加WebSocket处理
    server.addCallbackFormat(
        Format{"/wsconnect/%s", Format::Type::scanf},
        [](serverFile& file) {
            if (file.getAgreementType() == Protocol::HTTP) {
                if (WebSocketUtil::shouldbeUpdataToWS(file)) {
                    file.write(WebSocketUtil::makeWebSocketHandshake(file));
                    file.upgradeProtocol(Protocol::WebSocket);
                }
            } else if (file.getAgreementType() == Protocol::WebSocket) {
                // 处理WebSocket消息
                auto msg = file.getContent()["message"];
                if (!msg.empty()) {
                    std::cout << "收到消息: " << msg << std::endl;
                    // 广播消息给所有客户端
                    // ...
                }
            }
        });
    
    // 启动服务器
    auto& coManager = Co_Start_Manager::getInstance();
    coManager.getManager().add(server);
    coManager.setLoopInterval(std::chrono::milliseconds(1));
    std::cout << "服务器已启动，监听端口 8080..." << std::endl;
    coManager.start();
    
    return 0;
}
```
