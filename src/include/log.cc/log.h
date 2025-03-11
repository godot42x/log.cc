#pragma once

// #include "level.h"

#include <cassert>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <source_location>

#include <fstream>
#include <unordered_map>



#include "../base.h"


namespace __top_level_namespace
{



namespace LogLevel
{
enum T
{
    Debug = 100,
    Trace = 200,
    Info  = 300,
    Warn  = 400,
    Error = 500,
    Fatal = 600,
};

extern std::string_view toString(LogLevel::T level);


enum ETerminalColor
{
    Reset = 0,
    White,
    Green,
    Magenta,
    Cyan,
    Blue,
    Yellow,
    Red,
    ColorCount,
};

extern const std::unordered_map<LogLevel::T, std::string>    logLevel2Strings;            // LogLevel::Debug -> "DEBUG"
extern const std::unordered_map<LogLevel::T, std::string>    logLevel2CompatLevelStrings; // LogLevel::Debug -> "D"
extern const std::unordered_map<ETerminalColor, std::string> color2TerminalColorCode;     // ETerminalColor::Reset -> "\033[0m"
extern const std::unordered_map<LogLevel::T, ETerminalColor> level2TerminalColor;         // LogLevel::Debug -> ETerminalColor::Green
extern const std::unordered_map<LogLevel::T, std::string>    level2TerminalColorCode;     // LogLevel::Debug -> "\033[32m"


}; // namespace LogLevel



struct MessageQueue
{
    std::queue<std::string> queue;
    std::mutex              mutex;
    std::condition_variable conditionVar;
    bool                    bShutdown = false;

  public:
    void push(std::string_view msg)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.emplace(msg);
        // printf("push msg: %s\n", queue.back().c_str());
        conditionVar.notify_one();
    }

    bool pop(std::string &msg)
    {
        std::unique_lock<std::mutex> lock(mutex); // this will lock automatically?
        lock.lock();
        conditionVar.wait(lock, [this]() {
            printf("queue size: %zu\n, bShutdown: %d", queue.size(), bShutdown);
            return !queue.empty() || bShutdown;
        });
        if (bShutdown && queue.empty()) {
            return false;
        }

        // why? save it to file
        while (bShutdown && !queue.empty()) {
            msg = queue.front();
            queue.pop();
            return false;
        }

        msg = queue.front();
        queue.pop();
        return true;
    }

    void shutdown()
    {
        std::lock_guard<std::mutex> lock(mutex);
        bShutdown = true;
        conditionVar.notify_all();
    }
};

struct LOG_CC_API Config
{
    LogLevel::T logLevel       = LogLevel::Debug;
    LogLevel::T logDetailLevel = LogLevel::Warn; // With source location

    void setLogLevel(LogLevel::T level);
    void setLogDetailLevel(LogLevel::T level);

    static Config *get()
    {
        static Config config;
        return &config;
    }
};



enum class LogDetailFlags
{
    time     = 0 << 0,
    file     = 0 << 1,
    line     = 0 << 2,
    func     = 0 << 3,
    category = 0 << 4,
    level    = 0 << 5,
    msg      = 0 << 6,
};

struct LogAppender
{
    std::string   filename;
    std::ofstream fileStream;
};

struct LOG_CC_API Logger
{
    MessageQueue      msgQueue;
    std::atomic<bool> bExit;
    std::thread       workerThread;

    Config                   config;
    std::vector<LogAppender> appenders;
    bool                     bToStdOut = false;



    Logger() = default;

    ~Logger()
    {
        bExit = true;
        msgQueue.shutdown();
        msgQueue.conditionVar.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
        for (auto &appender : appenders) {
            if (appender.fileStream.is_open()) {
                appender.fileStream.close();
            }
        }
    }

    void runWorker()
    {
        // std::format("{}", 123);
        static constexpr int                                      flushIntervalSec = 10;
        static std::chrono::time_point<std::chrono::system_clock> last             = std::chrono::system_clock::now();

        workerThread = std::thread([this]() {
            std::string msg;
            while (msgQueue.pop(msg)) {
                printf("pop msg: %s\n", msg.c_str());
                for (auto &appender : appenders) {
                    appender.fileStream << msg.data();
                }
                int gap = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last).count();
                if (gap >= flushIntervalSec) {
                    for (auto &appender : appenders) {
                        appender.fileStream.flush();
                    }
                    last = std::chrono::system_clock::now();
                    printf("flush %d\n", gap);
                }
            }
        });
    }

    // TODO: custom format string to pass -> need a non const formatter?
    void log(LogLevel::T level, std::string_view msg, std::source_location location = std::source_location::current());
    void logWithCategory(LogLevel::T level, std::string_view category, std::string_view msg, std::source_location location = std::source_location::current());


    bool addAppender(const char *filename)
    {
        std::ofstream fileStream(filename, std::ios::out | std::ios::app);
        if (!fileStream.is_open()) {
            throw std::runtime_error("failed to open file");
            return false;
        }
        appenders.push_back({
            .filename   = filename,
            .fileStream = std::move(fileStream),
        });
        // printf("add appender: %s\n", filename);
        return true;
    }



  private:
    void doLog(LogLevel::T level, std::string_view msg);
};


struct LOG_CC_API LoggerFactory
{

    using Self = LoggerFactory;

    Logger *product = nullptr;

    LoggerFactory()
    {
        product = new Logger;
    }
    ~LoggerFactory()
    {
        if (product) {
            delete product;
        }
    }


    Self &addAppender(const char *filename)
    {
        product->addAppender(filename);
        return *this;
    }

    Self &setOuputToStdOut(bool bToStdOut)
    {
        product->bToStdOut = bToStdOut;
        return *this;
    }

    Logger *create()
    {
        Logger *that = product;
        product      = nullptr;
        return that;
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
