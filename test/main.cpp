#include "log.cc/log.h"

int main()
{
    using namespace __top_level_namespace;

    log(LogLevel::Debug, "test");
    log(LogLevel::Trace, "test");
    log(LogLevel::Info, "test");
    log(LogLevel::Warn, "test");
    log(LogLevel::Error, "test");
    log(LogLevel::Fatal, "test");

    log(LogLevel::Debug, FMT("test {}", 1));
    log(LogLevel::Trace, FMT("test {}", 2));
    log(LogLevel::Info, FMT("test {}", 3));
    log(LogLevel::Warn, FMT("test {}", 4));
    log(LogLevel::Error, FMT("test {}", 5));
    log(LogLevel::Fatal, FMT("test {}", 6));

    logWithCategory(LogLevel::Debug, "test", "test");
    logWithCategory(LogLevel::Trace, "test", "test");
    logWithCategory(LogLevel::Info, "test", "test");
    logWithCategory(LogLevel::Warn, "test", "test");
    logWithCategory(LogLevel::Error, "test", "test");
    logWithCategory(LogLevel::Fatal, "test", "test");

    return 0;
}