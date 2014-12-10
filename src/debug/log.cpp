#include <blons/debug/log.h>

// Includes
#include <iostream>
#include <memory>
#include <stdarg.h>

namespace
{
using namespace blons::log;

Level g_log_level = Level::NONE;
PrintCallback g_print = [](const std::string& s){ std::cout << s; };

void OutputLogMessage(std::string msg, Level log_level)
{
    if (log_level >= g_log_level)
    {
        g_print(msg);
    }
}

std::string FormatPrintString(const std::string& fmt, va_list args)
{
    size_t size = _vscprintf(fmt.c_str(), args) + 1;
    std::unique_ptr<char> buffer(new char[size]);
    vsnprintf_s(buffer.get(), size, size, fmt.c_str(), args);
    std::string formatted_string(buffer.get());

    return formatted_string;
}
} // namespace

namespace blons
{
namespace log
{
void Debug(const std::string& fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = FormatPrintString(fmt, args);
    va_end(args);

    OutputLogMessage(message, Level::DEBUG);
}

void Info(const std::string& fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = FormatPrintString(fmt, args);
    va_end(args);

    OutputLogMessage(message, Level::INFO);
}

void Warn(const std::string& fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = FormatPrintString(fmt, args);
    va_end(args);

    OutputLogMessage(message, Level::WARN);
}

void Fatal(const std::string& fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = FormatPrintString(fmt, args);
    va_end(args);

    OutputLogMessage(message, Level::FATAL);
}

void SetOutputLevel(Level level)
{
    g_log_level = level;
}

void SetPrintCallback(PrintCallback callback)
{
    g_print = callback;
}
} // namespace log
} // namespace blons