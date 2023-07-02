#pragma once
#include <unordered_map>

#define __top_level_namespace __logcpp


#if _WIN32 || !_HAS_CXX17
#include <fmt/std.h>
namespace __top_level_namespace {
using string_view = fmt::string_view;
}
#else
#include <string_view>
namespace __top_level_namespace {
using string_view = std::string_view;
}
#endif


namespace __top_level_namespace {


enum class LogLevel : int
{
    L_DEBUG = 0,
    L_TRACE,
    L_LOG,
    L_WARN,
    L_ERROR = 4,
    L_FATAL,
    L_LevelCount
};

inline static LogLevel LOG_LEVEL = LogLevel::L_DEBUG;
inline auto            SetLogLevel(LogLevel level) { LOG_LEVEL = level; }

inline char const *LogLevelStrings[] = {
    "DEBUG",
    "TRACE",
    "LOG",
    "WARN",
    "ERROR",
    "FATAL",
};

inline static std::unordered_map<std::string, std::string>
    TerminalColor =
        {
            {  "reset",  "\033[0m"},
            {  "white", "\033[37m"},
            {  "green", "\033[32m"},
            {"magenta", "\033[35m"},
            {   "cyan", "\033[36m"},
            {   "blue", "\033[34m"},
            {  "yello", "\033[33m"},
            {    "red", "\033[31m"},
};


inline auto get_terminal_color(LogLevel level)
{
    string_view color = TerminalColor["white"];

    switch (level) {
    case LogLevel::L_DEBUG:
        color = TerminalColor["cyan"];
        break;
    case LogLevel::L_TRACE:
    case LogLevel::L_LOG:
        color = TerminalColor["green"];
        break;
    case LogLevel::L_WARN:
        color = TerminalColor["yello"];
        break;
    case LogLevel::L_ERROR:
    case LogLevel::L_FATAL:
        color = TerminalColor["red"];
        break;
    case LogLevel::L_LevelCount:
        break;
    }
    return color;
}
} // namespace __top_level_namespace
