#include "log.cc/log.h"

#include <format>

#define FMT(fmt, ...) std::format(fmt __VA_OPT__(, )##__VA_ARGS__)

int main()
{

    using namespace __top_level_namespace;

    Logger *logger = LoggerFactory()
                         .setOuputToStdOut(true)
                         .addAppender("test.log")
                         .create();

    logger->runWorker();
    printf("123\n");


    logger->log(LogLevel::Debug, "test");
    logger->log(LogLevel::Trace, "test");
    logger->log(LogLevel::Info, "test");
    logger->log(LogLevel::Warn, "test");
    logger->log(LogLevel::Error, "test");
    logger->log(LogLevel::Fatal, "test");

    logger->log(LogLevel::Debug, std::format("test {}", 1));

    logger->log(LogLevel::Debug, FMT("test 2 {}", 1));
    logger->log(LogLevel::Trace, FMT("test 2 {}", 2));
    logger->log(LogLevel::Info, FMT("test 2 {}", 3));
    logger->log(LogLevel::Warn, FMT("test 2 {}", 4));
    logger->log(LogLevel::Error, FMT("test 2 {}", 5));
    logger->log(LogLevel::Fatal, FMT("test 2 {}", 6));


    logger->config.setLogDetailLevel(LogLevel::Debug);

    logger->logWithCategory(LogLevel::Debug, "test", "test");
    logger->logWithCategory(LogLevel::Trace, "test", "test");
    logger->logWithCategory(LogLevel::Info, "test", "test");
    logger->logWithCategory(LogLevel::Warn, "test", "test");
    logger->logWithCategory(LogLevel::Error, "test", "test");
    logger->logWithCategory(LogLevel::Fatal, "test", "test");



    delete logger;
    return 0;
}