#ifndef BLONSTECH_LOGGER_H_
#define BLONSTECH_LOGGER_H_

// Includes
#include <memory>
#include <stdarg.h>
#include <string>

class LoggerAPI
{
public:
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

protected:
    virtual void out(std::string msg, Level log_level)=0;
    Level log_level_ = Level::NONE;

private:
    std::string format(const char* fmt, va_list args);
};

extern std::unique_ptr<LoggerAPI> g_log;
#endif