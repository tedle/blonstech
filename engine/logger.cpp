#include "logger.h"

// Includes
#include <stdarg.h>

void LoggerAPI::Debug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = format(fmt, args);
    va_end(args);

    out(message, Level::DEBUG);
}

void LoggerAPI::Info(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = format(fmt, args);
    va_end(args);

    out(message, Level::INFO);
}

void LoggerAPI::Warn(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = format(fmt, args);
    va_end(args);

    out(message, Level::WARN);
}

void LoggerAPI::Fatal(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = format(fmt, args);
    va_end(args);

    out(message, Level::FATAL);
}

std::string LoggerAPI::format(const char* fmt, va_list args)
{
    size_t size = _vscprintf(fmt, args) + 1;
    std::unique_ptr<char> buffer(new char[size]);
    vsnprintf_s(buffer.get(), size, size, fmt, args);
    std::string formatted_string(buffer.get());

    return formatted_string;
}