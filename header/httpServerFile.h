#pragma once
#include "corutine.hpp"
#include "serverFile.h"
#include <cstring>
#include <file.h>
#include <format.h>
#include <functional>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class HttpServerFile : public serverFile
{
  public:
    friend serverFile;
    int fileState = true;
    // protocolType已移至基类
    int reset() final override;
    enum ParseState
    {
        REQUEST_LINE,
        HEADERS,
        BODY,
        COMPLETE
    };
    ParseState state = REQUEST_LINE;
    // for corutin
    std::string method;
    std::string path;
    std::string version;
    size_t content_length = 0;
    size_t body_read = 0;
    std::string body_buffer;

    // socketfile已移至基类
    std::function<void(serverFile&)> callback;
    int eventGo() override;

    void closeIt();
    virtual int getStatus() override final;
    virtual bool upgradeProtocol(int newProtocol) override;
    virtual bool resetCorutine() override; // 重写父类方法
    ~HttpServerFile() = default;
    HttpServerFile(int fd, std::function<void(serverFile&)> callback = nullptr);
    virtual void setCallback(std::function<void(serverFile&)> callback) final;
    virtual int handle();
    virtual void write(std::string file) final override;
    virtual std::map<std::string, std::string>& getContent() final; // 更新返回类型
    virtual const std::map<std::string, std::string>& getContent() const final; // 添加const版本

    // HTTP处理相关
    Task<void, void> httpEventloop() override; // 实现父类虚函数

    // WebSocket相关方法已移至WebSocketUtil
    // corutine已移至基类
};
