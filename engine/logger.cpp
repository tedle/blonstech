#include "logger.h"

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
    // Find the amount of memory we need to allocate for formatted string
    size_t size = vsnprintf(nullptr, 0, fmt, args) + 1;
    std::unique_ptr<char> buffer(new char[size]);
    vsnprintf(buffer.get(), size, fmt, args);
    std::string formatted_string(buffer.get());

    return formatted_string;
}