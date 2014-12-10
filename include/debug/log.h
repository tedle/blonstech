#ifndef BLONSTECH_DEBUG_LOGGER_H_
#define BLONSTECH_DEBUG_LOGGER_H_

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

void Debug(const char* fmt, ...);
void Info(const char* fmt, ...);
void Warn(const char* fmt, ...);
void Fatal(const char* fmt, ...);

void SetOutputLevel(Level level);
void SetPrintCallback(PrintCallback callback);
} // namespace log
} // namespace blons

#endif