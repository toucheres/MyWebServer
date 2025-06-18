#pragma once
#include "corutine.hpp"
#include "file.h"
#include "protocol_constants.h" // 新增包含
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

class serverFile : public co_async
{
  private:
    Protocol protocolType_ = Protocol::HTTP; // 更新类型和默认值
    std::map<std::string, std::string> content_;
    SocketFile socketfile_;
    Task<void, void> corutine_;
    std::function<void(serverFile&)> callback_;
    bool fileState_ = true;

  public:
    // 定义协议处理函数类型
    using ProtocolHandler = std::function<Task<void, void>(serverFile*)>;

    // 存储协议类型到处理函数的静态映射 - 使用懒加载方式
    static std::unordered_map<Protocol, ProtocolHandler>& getProtocolHandlers(); // 更新键类型

    // 注册协议处理函数
    static bool registerProtocolHandler(Protocol protocolType,
                                        ProtocolHandler handler); // 更新参数类型
    auto operator<<(auto&&... args)
    {
        return this->write(std::forward<decltype(args)>(args)...);
    }

  public:
    serverFile() = default;
    serverFile(int fd);

    // 实现所有虚函数，不再是抽象类
    Protocol getAgreementType() const; // 更新返回类型
    std::map<std::string, std::string>& getContent();
    const std::map<std::string, std::string>& getContent() const;
    void write(std::string file);
    void setCallback(std::function<void(serverFile&)> a_callback);
    bool getStatus() const;
    int handle();
    void closeIt();
    bool upgradeProtocol(Protocol newProtocol); // 更新参数类型

    // 协程管理
    bool resetCorutine();
    virtual EventStatus eventGo() override;

    // 派生类需要实现的用于协程重置的方法
    virtual Task<void, void> httpEventloop();

    virtual ~serverFile() = default;

    // Provide access to socketfile for derived classes or utilities if necessary
    SocketFile& getSocketFile()
    {
        return socketfile_;
    }
    const SocketFile& getSocketFile() const
    {
        return socketfile_;
    }

    // For httpEventloop and wsEventloop to modify fileState_
    void setFileState(bool state)
    {
        fileState_ = state;
    }
};