#ifndef BLONSTECH_DEBUG_CONSOLE_H_
#define BLONSTECH_DEBUG_CONSOLE_H_

// Includes
#include <functional>
#include <memory>
#include <vector>

namespace blons
{
// Since we use global state and singletons are pointlessly verbose,
// here's a bunch of namespaced functions instead!
namespace console
{
typedef std::function<void(const std::string&)> PrintCallback;

void in(const std::string& command);
void out(const std::string& fmt, ...);

void RegisterPrintCallback(PrintCallback callback);
} // namespace console
} // namespace blons
#endif