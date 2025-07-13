#pragma once
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>

namespace fs = std::filesystem;

class Log_inner
{
  private:
    Log_inner() = default;
    std::unique_ptr<std::ofstream> file_stream_;
    std::ostream* current_stream_ = &std::cerr;

  public:
    static inline Log_inner& getInstance()
    {
        static Log_inner log{};
        return log;
    }

    // 设置输出流
    void setOstream(std::ostream& stream)
    {
        current_stream_ = &stream;
    }

    // 设置文件输出
    bool setOstream(const std::string& filepath)
    {
        fs::path log_path(filepath);

        // 如果路径是目录，创建默认日志文件名
        if (fs::is_directory(log_path) || filepath.back() == '/')
        {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);

            std::string filename = std::format("log_{:%Y%m%d_%H%M%S}.txt",
                                               std::chrono::system_clock::from_time_t(time_t));
            log_path = log_path / filename;
        }

        // 创建父目录
        fs::path parent_dir = log_path.parent_path();
        if (!parent_dir.empty())
        {
            std::error_code ec;
            fs::create_directories(parent_dir, ec);
            if (ec)
            {
                std::cerr << "创建日志目录失败: " << ec.message() << std::endl;
                return false;
            }
        }

        // 打开文件
        file_stream_ = std::make_unique<std::ofstream>(log_path, std::ios::app);
        if (file_stream_->is_open())
        {
            current_stream_ = file_stream_.get();
            return true;
        }
        else
        {
            std::cerr << "打开日志文件失败: " << log_path << std::endl;
            return false;
        }
    }

    std::ostream& getStream()
    {
        return *current_stream_;
    }

    auto operator<<(auto&& args)
    {
        *current_stream_ << args;
        return *this;
    }
};

class log
{
  private:
    inline static Log_inner& log_inner = Log_inner::getInstance();

  public:
    enum class Level
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

  private:
    Level level_;

    std::string getLevelString(Level level)
    {
        switch (level)
        {
        case Level::DEBUG:
            return "DEBUG";
        case Level::INFO:
            return "INFO";
        case Level::WARNING:
            return "WARNING";
        case Level::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }

    std::string getCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        return std::format("{:%Y-%m-%d %H:%M:%S}.{:03d}",
                           std::chrono::system_clock::from_time_t(time_t), ms.count());
    }

  public:
    log(Level level = Level::INFO) : level_(level)
    {
        log_inner.getStream() << std::format("[{}] [{}] ", getCurrentTime(),
                                             getLevelString(level_));
    }

    ~log()
    {
        log_inner.getStream() << std::endl;
        log_inner.getStream().flush();
    }

    template <typename T> log& operator<<(T&& value)
    {
        log_inner.getStream() << std::forward<T>(value);
        return *this;
    }

    // 静态方法，方便使用
    static void setLogFile(const std::string& filepath)
    {
        log_inner.setOstream(filepath);
    }

    static void setLogStream(std::ostream& stream)
    {
        log_inner.setOstream(stream);
    }
    static void log_stdout_to(std::ostream& stream)
    {
        stream.flush();
        std::cout.rdbuf(stream.rdbuf());
    }
    static void log_stdout_to(std::string filepath = "logs/")
    {
        fs::path log_path(filepath);
        // 如果路径是目录，创建默认日志文件名
        if (fs::is_directory(log_path) || filepath.back() == '/')
        {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);

            std::string filename = std::format("cout_log_{:%Y%m%d_%H%M%S}.txt",
                                               std::chrono::system_clock::from_time_t(time_t));
            log_path = log_path / filename;
        }

        // 创建父目录
        fs::path parent_dir = log_path.parent_path();
        if (!parent_dir.empty())
        {
            std::error_code ec;
            fs::create_directories(parent_dir, ec);
            if (ec)
            {
                std::cerr << "创建日志目录失败: " << ec.message() << std::endl;
                return;
            }
        }
        std::ofstream current_stream_;
        // 打开文件
        std::ofstream file_stream_ = std::ofstream{log_path, std::ios::app};
        if (file_stream_.is_open())
        {
            current_stream_ = std::move(file_stream_);
        }
        else
        {
            std::cerr << "打开日志文件失败: " << log_path << std::endl;
            return;
        }
        std::cout.flush();
        std::cout.rdbuf(current_stream_.rdbuf());
    }
    static void log_stderr_to(std::ostream& stream)
    {
        stream.flush();
        std::cerr.rdbuf(stream.rdbuf());
    }
    static void log_stderr_to(std::string filepath = "logs/")
    {
        fs::path log_path(filepath);
        // 如果路径是目录，创建默认日志文件名
        if (fs::is_directory(log_path) || filepath.back() == '/')
        {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);

            std::string filename = std::format("cerr_log_{:%Y%m%d_%H%M%S}.txt",
                                               std::chrono::system_clock::from_time_t(time_t));
            log_path = log_path / filename;
        }

        // 创建父目录
        fs::path parent_dir = log_path.parent_path();
        if (!parent_dir.empty())
        {
            std::error_code ec;
            fs::create_directories(parent_dir, ec);
            if (ec)
            {
                std::cerr << "创建日志目录失败: " << ec.message() << std::endl;
                return;
            }
        }
        std::ofstream current_stream_;
        // 打开文件
        std::ofstream file_stream_ = std::ofstream{log_path, std::ios::app};
        if (file_stream_.is_open())
        {
            current_stream_ = std::move(file_stream_);
        }
        else
        {
            std::cerr << "打开日志文件失败: " << log_path << std::endl;
            return;
        }
        std::cerr.flush();
        std::cerr.rdbuf(current_stream_.rdbuf());
    }
};

// 便利宏
#define LOG_DEBUG log(log::Level::DEBUG)
#define LOG_INFO log(log::Level::INFO)
#define LOG_WARNING log(log::Level::WARNING)
#define LOG_ERROR log(log::Level::ERROR)