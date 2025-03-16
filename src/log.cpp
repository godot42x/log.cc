
#include <cassert>
#include <format>
#include <unordered_map>

#include "log.cc/log.h"


namespace __top_level_namespace
{


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

    return true;
}



} // namespace __top_level_namespace
