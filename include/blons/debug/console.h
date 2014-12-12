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

template <typename... Args>
void Register(const std::string& name, std::function<void(Args...)> func)
{
    internal::Function* f = new internal::TemplatedFunction<Args...>(func);
    internal::__register(name, f);
}

void RegisterPrintCallback(PrintCallback callback);

// Since template generation is done at compile time, user needs
// access to function generation code. You probably don't want
// to touch anything in here
#include <blons/debug/consolefunction.inl.h>
} // namespace console
} // namespace blons

#endif // BLONSTECH_DEBUG_CONSOLE_H_