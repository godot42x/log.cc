#pragma once

// #include "level.h"

#include <cassert>
#include <iostream>
#include <map>
#include <source_location>

#include <array>
#include <format>
#include <unordered_map>


#include "../base.h"

#define FMT(fmt, ...) (std::format(fmt __VA_OPT__(, )##__VA_ARGS__))

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

const inline std::unordered_map<LogLevel::T, std::string> logLevel2Strings = {
    {LogLevel::Debug, "DEBUG"},
    {LogLevel::Trace, "TRACE"},
    {LogLevel::Info, "Info"},
    {LogLevel::Warn, "WARN"},
    {LogLevel::Error, "ERROR"},
    {LogLevel::Fatal, "FATAL"},
};

const inline std::unordered_map<LogLevel::T, std::string> logLevel2CompatLevelStrings = {
    {LogLevel::Debug, "D"},
    {LogLevel::Trace, "T"},
    {LogLevel::Info, "I"},
    {LogLevel::Warn, "W"},
    {LogLevel::Error, "E"},
    {LogLevel::Fatal, "F"},
};

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



inline const std::unordered_map<ETerminalColor, std::string> color2TerminalColor = {
    {ETerminalColor::Reset, "\033[0m"},
    {ETerminalColor::White, "\033[37m"},
    {ETerminalColor::Green, "\033[32m"},
    {ETerminalColor::Magenta, "\033[35m"},
    {ETerminalColor::Cyan, "\033[36m"},
    {ETerminalColor::Blue, "\033[34m"},
    {ETerminalColor::Yellow, "\033[33m"},
    {ETerminalColor::Red, "\033[31m"},
};

inline const std::unordered_map<LogLevel::T, ETerminalColor> level2TerminalColor = {
    {LogLevel::Debug, ETerminalColor::Cyan},
    {LogLevel::Trace, ETerminalColor::White},
    {LogLevel::Info, ETerminalColor::Green},
    {LogLevel::Warn, ETerminalColor::Yellow},
    {LogLevel::Error, ETerminalColor::Red},
    {LogLevel::Fatal, ETerminalColor::Red},
};

template <typename K, typename V>
const V &unsafeConstMapGet(const std::unordered_map<K, V> &from, const K &k)
{
    const auto &it = from.find(k);
    return it->second;
}
template <typename K, typename V>
const V &unsafeConstMapGet(const std::map<K, V> &from, const K &k)
{
    const auto &it = from.find(k);
    return it->second;
}

inline const std::unordered_map<LogLevel::T, std::string> level2TerminalColorStr = {
    {LogLevel::Debug, unsafeConstMapGet(color2TerminalColor, unsafeConstMapGet(level2TerminalColor, LogLevel::Debug))},
    {LogLevel::Trace, unsafeConstMapGet(color2TerminalColor, unsafeConstMapGet(level2TerminalColor, LogLevel::Trace))},
    {LogLevel::Info, unsafeConstMapGet(color2TerminalColor, unsafeConstMapGet(level2TerminalColor, LogLevel::Info))},
    {LogLevel::Warn, unsafeConstMapGet(color2TerminalColor, unsafeConstMapGet(level2TerminalColor, LogLevel::Warn))},
    {LogLevel::Error, unsafeConstMapGet(color2TerminalColor, unsafeConstMapGet(level2TerminalColor, LogLevel::Error))},
    {LogLevel::Fatal, unsafeConstMapGet(color2TerminalColor, unsafeConstMapGet(level2TerminalColor, LogLevel::Fatal))},
};


}; // namespace LogLevel

static LogLevel::T logLevel       = LogLevel::Debug;
static LogLevel::T logDetailLevel = LogLevel::Warn; // With source location


static const std::string_view resetColor = LogLevel::unsafeConstMapGet(LogLevel::color2TerminalColor, LogLevel::ETerminalColor::Reset);


LOG_CC_API inline void log(LogLevel::T          level,
                           std::string_view     msg,
                           std::source_location location = std::source_location::current())
{
    if (level < logLevel) {
        return;
    }
    std::string      output;
    std::string_view levelStr = LogLevel::toString(level);
    output.resize(1024);


    // color
    std::string_view color = LogLevel::unsafeConstMapGet(LogLevel::level2TerminalColorStr, level);


    // clang-format off
    if (level >= logDetailLevel) {
        // TODO: custom format, let user define a macro?
        // [error] a/b/c/e.cpp:12:5 fooFunction: what msg
        output = FMT(
            "{}[{}]\t"
                "{}:{}:{} "
                "[{}]: "
                "{}"
                "{}\n",
                color, levelStr,
                location.file_name(), location.line(), location.column(),
                location.function_name(),
                msg,
                resetColor);

    }
    else {
        // [error] : what msg
        output = FMT(
            "{}[{}]\t"
                "{}"
                "{}\n",
                color, levelStr,
                msg,
                resetColor);
    }
    // clang-format on
    std::cout << output;
}



LOG_CC_API inline void logWithCategory(LogLevel::T      level,
                                       std::string_view category, std::string_view msg,
                                       std::source_location location = std::source_location::current())
{
    if (level < logLevel) {
        return;
    }
    std::string      output;
    std::string_view levelStr = LogLevel::toString(level);
    output.resize(1024);

    // color
    std::string_view color = LogLevel::unsafeConstMapGet(LogLevel::level2TerminalColorStr, level);

    // clang-format off
    // TODO: make a sub string constant, like categoryStr not need to be format at each time.
    if (level >= logDetailLevel) {
        output = FMT(
            "{}[{}]\t{}\t"
                "{}:{}:{} "
                "[{}]: "
                "{}"
                "{}\n",
                color, levelStr, category,
                location.file_name(), location.line(), location.column(),
                location.function_name(),
                msg,
                resetColor);
    }
    else {
        // (color)LogRender [error] : what msg(reset color)\n
        output = FMT(
            "{}[{}] {}"
                "{}"
                "{}\n",
                color, levelStr, category,
                msg,
                resetColor);
    }
    // clang-format on

    std::cout << output;
}



}; // namespace __top_level_namespace
