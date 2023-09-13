#pragma once
#include <map>
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
    L_DEBUG = 5,

    L_TRACE = 10,

    L_LOG = 15,

    L_PURE_ERROR = 19,

    // with details
    L_WARN  = 50,
    L_ERROR = 75,
    L_FATAL = 100,
};

inline static LogLevel LOG_LEVEL = LogLevel::L_DEBUG;
inline auto            SetLogLevel(LogLevel level) { LOG_LEVEL = level; }

const inline std::unordered_map<LogLevel, std::string> LogLevelStrings = {
    {     LogLevel::L_DEBUG,  "DEBUG"},
    {     LogLevel::L_TRACE,  "TRACE"},
    {       LogLevel::L_LOG,    "LOG"},
    {      LogLevel::L_WARN,   "WARN"},

    {LogLevel::L_PURE_ERROR, "PERROR"},

    {     LogLevel::L_ERROR,  "ERROR"},
    {     LogLevel::L_FATAL,  "FATAL"},
};

inline static std::map<std::string, std::string> TerminalColor =
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
        color = TerminalColor["white"];
        break;
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
    case LogLevel::L_PURE_ERROR:
        color = TerminalColor["magenta"];
        break;
    }
    return color;
}
} // namespace __top_level_namespace
