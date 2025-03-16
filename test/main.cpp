#include "log.cc/log.h"

#include <format>

#define FMT(fmt, ...) std::format(fmt __VA_OPT__(, )##__VA_ARGS__)

int func()
{

    using namespace __top_level_namespace;

    auto logCore = std::make_shared<LogCore>();
    logCore->addFileAppender({"test.log"});

    logCore->run();


    Logger logger(logCore);
    logger.setFormatter(DefaultFormatter{});

    // TODO: one worker for each logger
    printf("123\n");


    logger.log(LogLevel::Debug, "test");
    logger.log(LogLevel::Trace, "test");
    logger.log(LogLevel::Info, "test");
    logger.log(LogLevel::Warn, "test");
    logger.log(LogLevel::Error, "test");
    logger.log(LogLevel::Fatal, "test");

    logger.log(LogLevel::Debug, std::format("test {}", 1));

    logger.log(LogLevel::Debug, FMT("test 2 {}", 1));
    logger.log(LogLevel::Trace, FMT("test 2 {}", 2));
    logger.log(LogLevel::Info, FMT("test 2 {}", 3));
    logger.log(LogLevel::Warn, FMT("test 2 {}", 4));
    logger.log(LogLevel::Error, FMT("test 2 {}", 5));
    logger.log(LogLevel::Fatal, FMT("test 2 {}", 6));


    // debug(), "wtf!!!!!!";

    Logger logger2(logCore);
    logger2.setFormatter(CategoryFormatter{.category = "category-formatter"});

    // logger2.config.setLogDetailLevel(LogLevel::Debug);

    logger2.log(LogLevel::Debug, "test 3");
    logger2.log(LogLevel::Trace, "test 3");
    logger2.log(LogLevel::Info, "test 3");
    logger2.log(LogLevel::Warn, "test 3");
    logger2.log(LogLevel::Error, "test 3");
    logger2.log(LogLevel::Fatal, "test 3");


    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}

int main()
{
    func();

    return 0;
}