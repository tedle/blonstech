#include "debug/console.h"

// Includes
#include <stdarg.h>

namespace
{
using namespace blons::console;
struct ConsoleState
{
    std::vector<PrintCallback> print_callbacks;
} g_state;
} // namespace

namespace blons
{
namespace console
{
void in(const char* command)
{

}

void out(const char* fmt, ...)
{
    // Generate printf string
    va_list args;
    va_start(args, fmt);
    size_t size = _vscprintf(fmt, args) + 1;
    std::unique_ptr<char> buffer(new char[size]);
    vsnprintf_s(buffer.get(), size, size, fmt, args);
    std::string formatted_out(buffer.get());
    va_end(args);

    for (const auto& output : g_state.print_callbacks)
    {
        output(formatted_out);
    }
}

void RegisterPrintCallback(PrintCallback callback)
{
    g_state.print_callbacks.push_back(callback);
}
} // namespace console
} // namespace blons