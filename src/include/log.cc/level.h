#pragma once
#include <map>
#include <unordered_map>


#include <string>


#include <string_view>

#include "../base.h"


namespace __top_level_namespace
{

using string_view = std::string_view;

namespace LogLevel
{
enum T
{
    Debug = 5,

    Trace = 10,

    Log = 15,

    PureError = 19,

    // with details
    Warn  = 50,
    Error = 75,
    Fatal = 100,
};
};

inline static LogLevel::T LOG_LEVEL = LogLevel::Debug;
inline auto               SetLogLevel(LogLevel::T level) { LOG_LEVEL = level; }

const inline std::unordered_map<LogLevel::T, std::string> LogLevelStrings = {
    {LogLevel::Debug, "DEBUG"},
    {LogLevel::Trace, "TRACE"},
    {LogLevel::Log, "LOG"},
    {LogLevel::Warn, "WARN"},

    {LogLevel::PureError, "PERROR"},

    {LogLevel::Error, "ERROR"},
    {LogLevel::Fatal, "FATAL"},
};

inline static std::map<std::string, std::string> TerminalColor =
    {
        {"reset", "\033[0m"},
        {"white", "\033[37m"},
        {"green", "\033[32m"},
        {"magenta", "\033[35m"},
        {"cyan", "\033[36m"},
        {"blue", "\033[34m"},
        {"yello", "\033[33m"},
        {"red", "\033[31m"},
};


inline auto get_terminal_color(LogLevel::T level)
{
    string_view color = TerminalColor["white"];

    switch (level) {
    case LogLevel::Debug:
        color = TerminalColor["cyan"];
        break;
    case LogLevel::Trace:
        color = TerminalColor["white"];
        break;
    case LogLevel::Log:
        color = TerminalColor["green"];
        break;
    case LogLevel::Warn:
        color = TerminalColor["yello"];
        break;
    case LogLevel::Error:
    case LogLevel::Fatal:
        color = TerminalColor["red"];
        break;
    case LogLevel::PureError:
        color = TerminalColor["magenta"];
        break;
    }
    return color;
}
} // namespace __top_level_namespace
