
#include "log.h"

namespace __top_level_namespace {

#if 1

// void log_compat(LogLevel               level,
//                 const char            *time,
//                 char const            *file_name,
//                 const int              line,
//                 const char            *function_name,
//                 std::string_view const msg)
//{
//     if (level >= __top_level_namespace::LOG_LEVEL)
//     {
//
//         auto color = get_terminal_color(level);
//
//         try {
//             if (level < WARN) {
//                 auto out = fmt::format("{}[{}] {} | {} \n", color, LogLevelStrings[level], time, msg);
//                 std::cout << out;
//             }
//             else {
//                 auto out = fmt::format("{}[{}] {} | {}:{} | '{}' | {} \n", color, LogLevelStrings[level], time,
//                                        file_name, line, /*column*/ function_name,
//                                        msg);
//                 std::cerr << out;
//             }
//         }
//         catch (fmt::format_error &e) {
//             std::cerr << fmt::format("Format failed at {}:{} | {}", file_name, line, e.what()) << std::endl;
//         }
//         catch (std::exception &e) {
//             throw std::runtime_error(fmt::format("Exception on log at {}:{} | {}", file_name, line, e.what()));
//         }
//     }
// }

#else
#include <format>
#include <source_location>

#endif

} // namespace __top_level_namespace
