
#include "log.cc/log.h"
#include <format>

namespace __top_level_namespace
{

namespace LogLevel
{


std::string_view toString(LogLevel::T level)
{
    switch (level) {
    case Debug:
        return "Debug";
    case Trace:
        return "Trace";
    case Info:
        return "Info";
    case Warn:
        return "Warn";
    case Error:
        return "Error";
    case Fatal:
        return "Fatal";
    }
    assert(false);
    return "Unknown";
}


const std::unordered_map<LogLevel::T, std::string> logLevel2Strings = {
    {LogLevel::Debug, "DEBUG"},
    {LogLevel::Trace, "TRACE"},
    {LogLevel::Info, "Info"},
    {LogLevel::Warn, "WARN"},
    {LogLevel::Error, "ERROR"},
    {LogLevel::Fatal, "FATAL"},
};
const std::unordered_map<LogLevel::T, std::string> logLevel2CompatLevelStrings = {
    {LogLevel::Debug, "D"},
    {LogLevel::Trace, "T"},
    {LogLevel::Info, "I"},
    {LogLevel::Warn, "W"},
    {LogLevel::Error, "E"},
    {LogLevel::Fatal, "F"},
};


const std::unordered_map<ETerminalColor, std::string> color2TerminalColorCode = {
    {ETerminalColor::Reset, "\033[0m"},
    {ETerminalColor::White, "\033[37m"},
    {ETerminalColor::Green, "\033[32m"},
    {ETerminalColor::Magenta, "\033[35m"},
    {ETerminalColor::Cyan, "\033[36m"},
    {ETerminalColor::Blue, "\033[34m"},
    {ETerminalColor::Yellow, "\033[33m"},
    {ETerminalColor::Red, "\033[31m"},
};


const std::unordered_map<LogLevel::T, ETerminalColor> level2TerminalColor = {
    {LogLevel::Debug, ETerminalColor::Cyan},
    {LogLevel::Trace, ETerminalColor::White},
    {LogLevel::Info, ETerminalColor::Green},
    {LogLevel::Warn, ETerminalColor::Yellow},
    {LogLevel::Error, ETerminalColor::Red},
    {LogLevel::Fatal, ETerminalColor::Red},
};

template <typename T, typename K>
const auto &unsafeConstMapGet(const T &from, const K &k)
{
    return from.find(k)->second;
}


const std::unordered_map<LogLevel::T, std::string> level2TerminalColorCode = {
    {LogLevel::Debug, unsafeConstMapGet(color2TerminalColorCode, unsafeConstMapGet(level2TerminalColor, LogLevel::Debug))},
    {LogLevel::Trace, unsafeConstMapGet(color2TerminalColorCode, unsafeConstMapGet(level2TerminalColor, LogLevel::Trace))},
    {LogLevel::Info, unsafeConstMapGet(color2TerminalColorCode, unsafeConstMapGet(level2TerminalColor, LogLevel::Info))},
    {LogLevel::Warn, unsafeConstMapGet(color2TerminalColorCode, unsafeConstMapGet(level2TerminalColor, LogLevel::Warn))},
    {LogLevel::Error, unsafeConstMapGet(color2TerminalColorCode, unsafeConstMapGet(level2TerminalColor, LogLevel::Error))},
    {LogLevel::Fatal, unsafeConstMapGet(color2TerminalColorCode, unsafeConstMapGet(level2TerminalColor, LogLevel::Fatal))},
};

} // namespace LogLevel


static const std::string_view resetColor = LogLevel::unsafeConstMapGet(LogLevel::color2TerminalColorCode, LogLevel::ETerminalColor::Reset);

void Config::setLogLevel(LogLevel::T level)
{
    logLevel = level;
}

void Config::setLogDetailLevel(LogLevel::T level)
{
    logDetailLevel = level;
}



void Logger::log(LogLevel::T          level,
                 std::string_view     msg,
                 std::source_location location)
{
    if (level < config.logLevel) {
        return;
    }
    std::string      output;
    std::string_view levelStr = LogLevel::toString(level);
    output.resize(1024);


    // color
    std::string_view color = LogLevel::unsafeConstMapGet(LogLevel::level2TerminalColorCode, level);


    // clang-format off
    if (level >= config.logDetailLevel) {
        // TODO: custom format, let user define a macro?
        // [error] a/b/c/e.cpp:12:5 fooFunction: what msg
        output = std::format(
            "[{}]\t"
                "{}:{}:{} "
                "[{}]: "
                "{}\n",
                levelStr,
                location.file_name(), location.line(), location.column(),
                location.function_name(),
                msg);

    }
    else {
        // [error] : what msg
        output = std::format(
            "[{}]\t"
                "{}"
                "\n",
                levelStr,
                msg);
    }
    // clang-format on
    doLog(level, output);
}

void Logger::logWithCategory(LogLevel::T      level,
                             std::string_view category, std::string_view msg,
                             std::source_location location)
{
    if (level < config.logLevel) {
        return;
    }
    std::string      output;
    std::string_view levelStr = LogLevel::toString(level);
    output.resize(1024);

    // color
    std::string_view color = LogLevel::unsafeConstMapGet(LogLevel::level2TerminalColorCode, level);

    // clang-format off
    // TODO: make a sub string constant, like categoryStr not need to be format at each time.
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
                color, levelStr, category,
                msg);
    }
    // clang-format on

    doLog(level, output);
}

void Logger::doLog(LogLevel::T level, std::string_view msg)
{
    msgQueue.push(msg);
    if (bToStdOut)
    {
        std::string_view              color      = LogLevel::unsafeConstMapGet(LogLevel::level2TerminalColorCode, level);
        static const std::string_view resetColor = LogLevel::unsafeConstMapGet(LogLevel::color2TerminalColorCode, LogLevel::ETerminalColor::Reset);
        std::cout << color << msg.data() << resetColor;
    }
}



} // namespace __top_level_namespace
