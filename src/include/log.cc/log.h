#pragma once

// #include "level.h"

#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <set>
#include <source_location>

#include <fstream>
#include <unordered_map>



#include "../base.h"
#include "log_level.h"



namespace __top_level_namespace
{


inline std::string getCurentTimeStr()
{
    auto now = std::chrono::zoned_time{
        std::chrono::current_zone(),
        std::chrono::system_clock::now(),
    };

    return std::format("{:%Y-%m-%d %H:%M:%S}", now);
}


struct Logger;

struct MessageQueue
{
    struct Elem
    {
        LogLevel::T level;
        std::string msg;
    };

    std::queue<Elem>        queue;
    std::mutex              mutex;
    std::condition_variable cv;
    bool                    bShutdown = false;

  public:


    void push(std::string_view msg, LogLevel::T level = LogLevel::Info)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.emplace(Elem{
            .level = level,
            .msg   = std::string(msg),
        });
        cv.notify_one();
    }
    void push(std::string &&msg, LogLevel::T level = LogLevel::Info)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.emplace(Elem{
            .level = level,
            .msg   = std::move(msg),
        });
        cv.notify_one();
    }

    void push(Elem &&elem)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.emplace(std::move(elem));
        cv.notify_one();
    }

    bool pop(Elem &elem)
    {
        std::unique_lock<std::mutex> lock(mutex); // this will lock automatically! double lock cause a error
        cv.wait(lock, [this]() {
            // printf("queue size: %zu, bShutdown: %d", queue.size(), bShutdown);
            return !queue.empty() || bShutdown;
        });
        if (bShutdown && queue.empty()) {
            return false;
        }

        // why? save it to file
        while (bShutdown && !queue.empty()) {
            elem = queue.front();
            queue.pop();
            return false;
        }

        elem = std::move(queue.front());
        queue.pop();
        return true;
    }


    void shutdown()
    {
        std::lock_guard<std::mutex> lock(mutex);
        bShutdown = true;
        cv.notify_all();
    }
};

struct LOG_CC_API Config
{
    LogLevel::T logLevel       = LogLevel::Debug;
    LogLevel::T logDetailLevel = LogLevel::Warn; // With source location

    void setLogLevel(LogLevel::T level);
    void setLogDetailLevel(LogLevel::T level);
};



struct ConsoleAppender
{
    std::ostream &out = std::cout;

    void operator()(const MessageQueue::Elem &elem)
    {
        static const std::string_view resetColor = LogLevel::color2TerminalColorCode.find(LogLevel::ETerminalColor::Reset)->second;
        const std::string_view        color      = LogLevel::level2TerminalColorCode.find(elem.level)->second;
        out << color << elem.msg << resetColor;
    }
};

struct FileAppender
{
    std::string   filename;
    std::ofstream fileStream;

    FileAppender(FileAppender &&other)
    {
        filename   = std::move(other.filename);
        fileStream = std::move(other.fileStream);
    }

    FileAppender(std::string_view filename)
    {
        this->filename = std::string(filename);
        fileStream     = std::ofstream(std::string(filename), std::ios::out | std::ios::app);
    }

    void operator()(const MessageQueue::Elem &elem)
    {
        fileStream << elem.msg;
    }
    void flush()
    {
        fileStream.flush();
    }

    ~FileAppender()
    {
        fileStream.close();
    }
};



struct LOG_CC_API DefaultFormatter
{
    bool operator()(const Config &config, std::string &output, LogLevel::T level, std::string_view msg, const std::source_location &location);
};

struct LOG_CC_API CategoryFormatter
{
    std::string category;

    bool operator()(const Config &config, std::string &output, LogLevel::T level, std::string_view msg, const std::source_location &location)
    {
        std::string_view levelStr = LogLevel::toString(level);


        // clang-format off
        if (level >= config.logDetailLevel) {
            output = std::format(
                "[{}]\t{}\t"
                    "{}:{}:{} "
                    "[{}]: "
                    "{}\n",
                    levelStr, category,
                    location.file_name(), location.line(), location.column(),
                    location.function_name(),
                    msg);
        }
        else {
            // (color)LogRender [error] : what msg(reset color)\n
            output = std::format(
                "[{}] {}"
                    "{}\n",
                    levelStr, category,
                    msg);
        }
        // clang-format on

        return true;
    }
};


struct LogCore
{
    std::vector<FileAppender> fileAppenders;
    ConsoleAppender           consoleAppender;

    std::thread  workerThread;
    MessageQueue msgQueue;


    void internalLog(std::string_view msg)
    {
        std::cout << "log.cc LogProcessor " << msg << '\n';
    }

    ~LogCore()
    {
        msgQueue.shutdown();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    void run()
    {
        // std::format("{}", 123);
        static constexpr int flushIntervalSec = 10;

        auto flushTask = [this]() {
            static auto last = std::chrono::system_clock::now();
            int         gap  = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last).count();
            if (gap >= flushIntervalSec) {
                for (auto &fileAppender : fileAppenders) {
                    fileAppender.flush();
                    internalLog(std::format(" {} log thread flushed {} , gap: {}\n", getCurentTimeStr(), fileAppender.filename, gap));
                }
                last = std::chrono::system_clock::now();
            }
        };


        workerThread = std::thread([this, flushTask]() {
            static MessageQueue::Elem elem;
            while (msgQueue.pop(elem)) {
                // internalLog(std::format("pop msg:{} \n", elem.msg.c_str()));
                for (auto &fileAppender : fileAppenders) {
                    fileAppender(elem);
                }

                consoleAppender(elem);

                flushTask();
            }
        });
    }

    void push(std::string &&msg, LogLevel::T level = LogLevel::Info)
    {
        msgQueue.push(std::move(msg), level);
    }

    void addFileAppender(std::string_view filename)
    {
        // auto ap = FileAppender(filename);
        // fileAppenders.push_back(std::move(ap));
        fileAppenders.emplace_back(filename);
    }
};



struct LOG_CC_API Logger
{
    std::shared_ptr<LogCore> logCore;
    Config                   config;


    using formatter_t     = std::function<bool(const Config &config, std::string &output, LogLevel::T, std::string_view, const std::source_location &)>;
    formatter_t formatter = nullptr;


    Logger(std::shared_ptr<LogCore> logCore)
    {
        this->logCore = logCore;
        if (!formatter) {
            formatter = DefaultFormatter{};
        }
    }


    void setFormatter(formatter_t formatter_)
    {
        formatter = formatter_;
    }


    void log(LogLevel::T level, std::string_view msg, std::source_location location = std::source_location::current())
    {
        if (level < config.logLevel) {
            return;
        }
        std::string output(512, '\0');
        if (formatter(config, output, level, msg, location)) {
            logCore->push(std::move(output), level);
        }
    }


  protected:


    void internalLog(std::string_view msg)
    {
        std::cout << "log.cc Logger " << msg << '\n';
    }
};


struct debug
{
    std::ostream &out = std::cout;

    debug &operator,(auto msg)
    {
        out << msg << '\n';
        return *this;
    }

    ~debug()
    {
        out << '\n';
    }
};


}; // namespace __top_level_namespace
