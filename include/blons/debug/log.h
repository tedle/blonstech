#ifndef BLONSTECH_DEBUG_LOG_H_
#define BLONSTECH_DEBUG_LOG_H_

// Includes
#include <functional>
#include <string>

namespace blons
{
namespace log
{
typedef std::function<void(const std::string&)> PrintCallback;
enum Level {
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    FATAL = 4,
    NONE = 5
};

void Debug(const std::string& fmt, ...);
void Info(const std::string& fmt, ...);
void Warn(const std::string& fmt, ...);
void Fatal(const std::string& fmt, ...);

void SetOutputLevel(Level level);
void SetPrintCallback(PrintCallback callback);
} // namespace log
} // namespace blons

#endif // BLONSTECH_DEBUG_LOG_H_