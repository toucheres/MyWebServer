#pragma once
#include <corutine.hpp>
#include <cstddef>
#include <memory>
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
    std::string_view test;

  public:
    bool load(std::string& path);
    std::string_view read();
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
        int fd=-1;
        std::vector<char> content={};
        mutable size_t left = 0;
        mutable size_t right = 0;
    };
    static Task<> eventfun(std::shared_ptr<CONTEXT> context);

  public:
    Task_Local<CONTEXT> handle = eventfun;
    virtual int eventGo() final;
    const std::string_view read_added() const;
    const std::string_view read_line() const;
    const std::string_view read_all() const;
    bool setNonBlocking();
    bool load(int a_fd);
    SocketFile(int a_fd);
    ~SocketFile();
    SocketFile()=default;
    SocketFile(SocketFile&& move);
    SocketFile& operator=(SocketFile && move);
    // SocketFile(const SocketFile& copy);
};
struct LocalFiles
{
  private:
    std::unordered_map<std::string, LocalFile> fileMap;

  public:
    bool add(std::string& path);
    LocalFile& get(std::string& path);
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
