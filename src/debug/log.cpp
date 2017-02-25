////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/debug/log.h>

// Includes
#include <iostream>
#include <memory>
#include <stdarg.h>

namespace blons
{
namespace log
{
namespace
{
Level g_log_level = Level::INFO;
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

void Debug(const std::string fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = FormatPrintString(fmt, args);
    va_end(args);

    OutputLogMessage(message, Level::DEBUG);
}

void Info(const std::string fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = FormatPrintString(fmt, args);
    va_end(args);

    OutputLogMessage(message, Level::INFO);
}

void Warn(const std::string fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string message = FormatPrintString(fmt, args);
    va_end(args);

    OutputLogMessage(message, Level::WARN);
}

void Fatal(const std::string fmt, ...)
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