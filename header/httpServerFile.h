#pragma once
#include "corutine.hpp"
#include "serverFile.h"
#include "webSocketFile.h"
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
class WebSocketFile;
class HttpServerFile : public serverFile
{
    friend serverFile;
    friend WebSocketFile;
    int httpState = true;
    void reset();
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
    std::function<void(HttpServerFile&)> callback;
    int eventGo() override;

  public:
    void closeIt();
    virtual int getStatus() override final;
    ~HttpServerFile() = default;
    HttpServerFile(int fd, std::function<void(HttpServerFile&)> callback = nullptr);
    virtual void setCallback(std::function<void(serverFile&)> callback) final;
    virtual int handle();
    virtual void write(std::string file) final;
    virtual const std::map<std::string, std::string>& getContent() final;
    Task<void, void> eventloop();
    Task<void, void> corutine = eventloop();
};
