////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
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

#ifndef BLONSTECH_DEBUG_LOG_H_
#define BLONSTECH_DEBUG_LOG_H_

// Includes
#include <functional>
#include <string>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Functions for debug logging.
////////////////////////////////////////////////////////////////////////////////
namespace log
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Prototype for functions called everytime the logger prints.
///
/// The input string contains text that is outputted by the logger.
////////////////////////////////////////////////////////////////////////////////
typedef std::function<void(const std::string&)> PrintCallback;

////////////////////////////////////////////////////////////////////////////////
/// \brief Debug logging levels
///
/// The lower the log level the more is printed. Ordered:
/// * `NONE`
/// * `FATAL`
/// * `WARN`
/// * `INFO`
/// * `DEBUG`
///
/// \see SetOutputLevel
////////////////////////////////////////////////////////////////////////////////
enum Level {
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    FATAL = 4,
    NONE = 5
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Logs debug messages
///
/// Log level must be `DEBUG` for these messages to be printed.
////////////////////////////////////////////////////////////////////////////////
void Debug(const std::string& fmt, ...);

////////////////////////////////////////////////////////////////////////////////
/// \brief Logs info messages
///
/// Log level must be `INFO` or lower for these messages to be printed.
////////////////////////////////////////////////////////////////////////////////
void Info(const std::string& fmt, ...);

////////////////////////////////////////////////////////////////////////////////
/// \brief Logs warning messages
///
/// Log level must be `WARN` or lower for these messages to be printed.
////////////////////////////////////////////////////////////////////////////////
void Warn(const std::string& fmt, ...);

////////////////////////////////////////////////////////////////////////////////
/// \brief Logs fatal messages
///
/// Log level must be `FATAL` or lower for these messages to be printed.
////////////////////////////////////////////////////////////////////////////////
void Fatal(const std::string& fmt, ...);

////////////////////////////////////////////////////////////////////////////////
/// \brief Sets the debug output level for all subsequent calls to logging
/// functions.
///
/// Default at program start is `INFO`.
///
/// \see Level
////////////////////////////////////////////////////////////////////////////////
void SetOutputLevel(Level level);

////////////////////////////////////////////////////////////////////////////////
/// \brief Sets the function that will be called whenever the logger prints.
///
/// The callback function will be sent a `const std::string&` containing
/// the text that is being printed each call. There can only be a single
/// callback stored at a time.
///
/// Default at program start prints to stdout.
///
/// \param callback Function to be called when the logger prints
////////////////////////////////////////////////////////////////////////////////
void SetPrintCallback(PrintCallback callback);
} // namespace log
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \namespace blons::log
/// \ingroup debug
///
/// Allows for various logging levels and customizable output.
///
/// ### Example:
/// \code
/// // Output
/// blons::log::Debug("Printing to stdout!\n");
///
/// // Custom logging
/// blons::log::PrintCallback print = [](const std::string& out)
/// {
///     std::cout << "[LOGGING]: " << out;
/// }
/// blons::log::SetPrintCallback(print);
/// blons::log::Debug("Printing with a cool tag!\n");
///
/// // Using log levels
/// blons::log::SetOutputLevel(blons::log::WARN); // Default is INFO
/// blons::log::Debug("This is not printed!\n");
/// blons::log::Info("This is not printed!\n");
/// blons::log::Warn("This is printed!\n");
/// blons::log::Fatal("This is printed!\n");
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_DEBUG_LOG_H_