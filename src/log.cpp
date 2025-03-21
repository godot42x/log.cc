
#include <cassert>
#include <format>
#include <unordered_map>

#include "log.h"


TOP_LEVEL_NAMESPACE_BEGIN


std::string getCurentTimeStr()
{
    auto now = std::chrono::zoned_time{
        std::chrono::current_zone(),
        std::chrono::system_clock::now(),
    };

    return std::format("{:%Y-%m-%d %H:%M:%S}", now);
}

void Config::setLogLevel(LogLevel::T level)
{
    logLevel = level;
}

void Config::setLogDetailLevel(LogLevel::T level)
{
    logDetailLevel = level;
}



bool DefaultFormatter::operator()(const Config &config, std::string &output, LogLevel::T level, std::string_view msg, const std::source_location &location)
{

    std::string_view levelStr = LogLevel::level2Strings.find(level)->second;
    // clang-format off
    if (level >= config.logDetailLevel) {
        // TODO: custom format, let user define a macro?
        output = std::format(
            "[{}]\t"
                "{}:{} "
                "{}\n",
                levelStr,
                location.file_name(), location.line(),
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

    return true;
}



bool CategoryFormatter::operator()(const Config &config, std::string &output, LogLevel::T level, std::string_view msg, const std::source_location &location)
{
    std::string_view levelStr = LogLevel::toString(level);


    // clang-format off
    if (level >= config.logDetailLevel) {
        output = std::format(
            "[{}]\t{} "
                "{}:{} "
                "{}\n",
                levelStr, category,
                location.file_name(), location.line(),
                msg);
    }
    else {
        // (color)LogRender [error] : what msg(reset color)\n
        output = std::format(
            "[{}]\t{} "
                "{}\n",
                levelStr, category,
                msg);
    }
    // clang-format on

    return true;
}


TOP_LEVEL_NAMESPACE_END