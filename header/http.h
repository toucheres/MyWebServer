#pragma once
#include "corutine.hpp"
#include <cstring>
#include <file.h>
#include <functional>
#include <map>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
class HttpFiles;
class HttpFile;
class HttpServer;
class HttpAPI
{
    LocalFiles& static_files;
    HttpFile& socket;
    std::string_view getUrl();
    std::string_view getContext(std::string_view key);
    void write(std::string_view context);
    HttpAPI(LocalFiles& files, HttpFile& in);
};
class HttpFile : public co_async
{
    friend HttpFiles;
    friend HttpAPI; // 修改为正确引用嵌套类
    SocketFile socketfile;
    std::map<std::string_view, std::string_view> content;
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

    std::string method;
    std::string path;
    std::string version;
    size_t content_length = 0;
    size_t body_read = 0;
    std::string body_buffer;

  public:
    int eventGo() override;
    HttpFile(int fd);
    virtual int handle();
    Task<void, void> eventloop();
    Task<void, void> corutine = eventloop();
};
struct HttpFiles : public co_async
{
  private:
    std::unordered_map<int, HttpFile> fileMap;

  public:
    virtual int eventGo() override;
    bool add(int fd);
    HttpFile& get(int fd);
    const std::unordered_map<int, HttpFile>& getMap();
};

class HttpServer : public co_async
{
  private:
    int server_fd;
    uint16_t port;
    std::string ip_listening;
    bool running;
    HttpFiles sockets;
    Co_Manager manager;
    //std::unordered_map<std::string, HttpAPI> callback;
    // 创建套接字
    int eventGo() override;
    int makeSocket();
    int bindSocket(int server_fd);
    bool listenSocket(int server_fd, size_t listenLenth = 5);
    int AcceptSocket(int server_fd, struct sockaddr* client_addr, socklen_t* client_addr_len);
    bool setReuseAddr(int& fd);
    Task<void, void> start();
    Task<void, void> handle = start();
    std::string processRequest(const std::string& request);
    std::map<std::string, std::function<void(HttpAPI)>> callbacks;
    std::map<std::string, std::function<void(HttpAPI)>> callbacks_format;

  public:
    HttpServer(std::string ip_listening = "0.0.0.0", uint16_t port = 8080);
    ~HttpServer();
    bool stop();
    void addCallback(std::string path, std::function<void(HttpAPI)> callback);
    void addCallbackFormat(std::string format, std::function<void(HttpAPI)> callback);
    void handleClient(int client_fd);
};
