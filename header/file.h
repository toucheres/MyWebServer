#pragma once
#include <corutine.hpp>
#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
struct LocalFiles;
struct SocketFiles;
struct LocalFile
{
    friend LocalFiles;

  private:
    std::vector<char> content;
    std::string path = "";
    size_t size = 0;
    std::string_view fileview;

  public:
    bool load(std::string& path);
    std::string_view read();
    // std::string_view wirte();
    LocalFile(std::string a_path);
    LocalFile(LocalFile&& move);
};

struct SocketFile : public co_async
{
    friend SocketFiles;

  private:
    class CONTEXT
    {
      public:
        friend SocketFile;
        int fd = -1;
        // read
        std::vector<char> content = {};
        mutable size_t r_left = 0;
        mutable size_t r_right = 0;
        //  write
        struct writingFile
        {
            std::string waitingWrite = {};
            mutable size_t w_left = 0;
            mutable size_t w_right = 0;
            writingFile(std::string&& move);
            writingFile(const std::string& copy);
        };

        std::queue<writingFile> waitingWrites;

        int fd_state = OK;
    };
    static Task<> eventfun(std::shared_ptr<CONTEXT> context);

  public:
    Task_Away<CONTEXT> handle = eventfun;
    virtual int eventGo() final;
    const std::string_view read_added() const;
    const std::string_view read_line() const;
    const std::string_view read_until(
        const std::string_view delimiter = "\r\n") const; // 添加新方法
    const std::string_view read_all() const;
    const void writeFile(std::string file);
    bool setNonBlocking();
    void closeIt();
    bool load(int a_fd);
    SocketFile(int a_fd);
    ~SocketFile();
    SocketFile() = default;
    SocketFile(SocketFile&& move);
    SocketFile& operator=(SocketFile&& move);
    enum fd_state
    {
        UNKOWN = -1,
        OK = 0,
        WRONG = 1,
    };
    // SocketFile(const SocketFile& copy);
};
struct async_in_out : public co_async
{
  private:
    SocketFile in{STDIN_FILENO};
    SocketFile out{STDOUT_FILENO};
    virtual int eventGo();

  public:
    async_in_out();
    std::string_view read_added();
    std::string_view read_line();
    std::string_view read_all();
    std::string_view read_until();
    void writeFile(std::string file);
    static async_in_out& getInstance();
};
struct LocalFiles
{
  private:
    std::unordered_map<std::string, LocalFile> fileMap;

  public:
    bool add(std::string& path);
    LocalFile& get(const std::string& path);
};
struct SocketFiles : public co_async
{
  private:
    std::unordered_map<int, SocketFile> fileMap;

  public:
    virtual int eventGo() override;
    bool add(int fd);
    SocketFile& get(int fd);
    const std::unordered_map<int, SocketFile>& getMap();
};
