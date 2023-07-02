#pragma once

#include <exception>
#include <iostream>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "level.h"

#include "microshit_api_log.h"


namespace __top_level_namespace {

#if _WIN32 | !_HAS_CXX20
#include "fmt/core.h"
#include "fmt/format.h"



void LOG_API inline log_compat(LogLevel          level,
                               const char       *time,
                               char const       *file_name,
                               const int         line,
                               const char       *function_name,
                               string_view const msg)
{
    if (level >= __top_level_namespace::LOG_LEVEL)
    {
        auto color = get_terminal_color(level);


        try {
            if (level < LogLevel::L_WARN) {
                auto out = fmt::format("{}[{}] '{}' {} | {} \n", color, LogLevelStrings.at(level), function_name, time, msg);
                std::cout << out;
            }
            else {
                auto out = fmt::format("{}[{}] {} | {}:{} | '{}' | {}\n", color, LogLevelStrings.at(level), time,
                                       file_name, line, /*column*/ function_name,
                                       msg);
                std::cerr << out;
            }
        }
        catch (fmt::format_error &e) {
            std::cerr << fmt::format("Format failed at {}:{} | {}", file_name, line, e.what()) << std::endl;
        }
        catch (std::exception &e) {
            throw std::runtime_error(fmt::format("Exception on log at {}:{} | {}", file_name, line, e.what()));
        }
    }
}



#define __FORMAT_LOG(level, fmts, ...)                                                         \
    do {                                                                                       \
        std::string msg = fmt::format(fmts __VA_OPT__(, ) __VA_ARGS__);                        \
        __top_level_namespace::log_compat(level, __TIME__, __FILE__, __LINE__, __func__, msg); \
    } while (0)

#else

#include <format>
#include <source_location>

#define __FORMAT_LOG(level, fmt, ...)                                  \
    do {                                                               \
        std::string msg = std::format(fmt __VA_OPT__(, ) __VA_ARGS__); \
        __top_level_namespace::log(level, __TIME__, msg);              \
    } while (0)

// @zenovis:
// https://github.com/zenustech/zeno/blob/master/zeno/include/zeno/utils/log.h
template <class T>
class LOG_API __with_source_location
{
    using source_location = std::source_location;

  public:
    template <class Value>
    __with_source_location(Value &&val, source_location loc = source_location::current()) : m_value(std::forward<Value>(val)), m_loc(loc) {}

    [[nodiscard]] auto const &value() const { return m_value; }
    auto                     &value() { return m_value; }
    [[nodiscard]] auto const &location() const { return m_loc; }
    auto                     &location() { return m_loc; }

  private:
    T m_value;
    // source_location m_loc
    std::source_location m_loc;
};



template <class... Args>
void LOG_API log(LogLevel                                        level,
                 const char                                     *time,
                 __with_source_location<std::string_view> const &msg)
{
    if (level >= __top_level_namespace::LOG_LEVEL)
    {

        auto color = get_terminal_color(level);

        try {
            if (level < WARN) {
                auto out = std::format("{}[{}] {} | {} \n", color, LogLevelStrings[level], time, msg.value());
                std::cout << out;
            }
            else {
                auto out = std::format("{}[{}] | {} {}:{}:{} | '{}' | {} \n", color, LogLevelStrings[level], time,
                                       msg.location().file_name(), msg.location().line(), msg.location().column(), msg.location().function_name(),
                                       msg.value());
                std::cerr << out;
            }
        }
        catch (std::format_error &e) {
            auto &&filename = msg.location().file_name();
            auto &&line     = msg.location().line();
            std::cerr << std::foramt("Format failed at {}:{} | {}", filename, line, e.what()) << std::endl;
        }
        catch (std::exception &e) {
            throw std::runtime_error(std::format("Exception on log at {}:{} | {}", msg.location().file_name(), msg.location().line(), e.what()));
        }
    }
}


#endif
}; // namespace __top_level_namespace



#define LOG(...) __FORMAT_LOG(__top_level_namespace::LogLevel::L_LOG, __VA_ARGS__)
#define TRACE(...) __FORMAT_LOG(__top_level_namespace::LogLevel::L_TRACE, __VA_ARGS__)
#define DEBUG(...) __FORMAT_LOG(__top_level_namespace::LogLevel::L_DEBUG, __VA_ARGS__)
#define WARN(...) __FORMAT_LOG(__top_level_namespace::LogLevel::L_WARN, __VA_ARGS__)
#define LOG_ERROR(...) __FORMAT_LOG(__top_level_namespace::LogLevel::L_ERROR, __VA_ARGS__)
#define LOG_PURE_ERROR(...) __FORMAT_LOG(__top_level_namespace::LogLevel::L_PURE_ERROR, __VA_ARGS__)
#define LOG_FATAL(...) __FORMAT_LOG(__top_level_namespace::LogLevel::L_FATAL, __VA_ARGS__)
