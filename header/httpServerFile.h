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

    std::map<std::string, std::string> content;
    SocketFile socketfile;
    std::function<void(serverFile&)> callback;
    int eventGo() override;

    // WebSocket 相关函数
    Task<void, void> wsEventloop(); // WebSocket事件循环

    void closeIt();
    virtual int getStatus() override final;
    //virtual int getAgreementType() override; // 仍然需要覆盖实现以支持特殊逻辑
    virtual bool upgradeProtocol(int newProtocol) override;
    virtual bool resetCorutine() override;
    ~HttpServerFile() = default;
    HttpServerFile(int fd, std::function<void(serverFile&)> callback = nullptr);
    virtual void setCallback(std::function<void(serverFile&)> callback) final;
    virtual int handle();
    virtual void write(std::string file) final;
    virtual const std::map<std::string, std::string>& getContent() const final;

    // HTTP处理相关
    Task<void, void> httpEventloop(); // HTTP事件循环

    // WebSocket相关
    static bool shouldbeUpdataToWS(const serverFile& httpfile); // 从WebSocketFile移动的函数

    Task<void, void> corutine; // 当前活动的协程
};
