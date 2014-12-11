#include <blons/debug/console.h>

// Includes
#include <stdarg.h>
// Local Includes
#include "consoleparser.h"

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
void in(const std::string& command)
{
    out("~ " + command + '\n');

    // Spaghetti parser is go
    std::vector<ConsoleArg> args;
    try
    {
        args = ParseCommand(command);
    }
    catch (const char* error)
    {
        out("%s in: %s\n", error, command.c_str());
    }

    out("<");
    for (const auto& t : args)
    {
        out("(t:");
        switch (t.type)
        {
        case ConsoleArg::NONE:
            out("n");
            break;
        case ConsoleArg::FUNCTION:
            out("c");
            break;
        case ConsoleArg::STRING:
            out("s");
            break;
        case ConsoleArg::INT:
            out("i");
            break;
        case ConsoleArg::FLOAT:
            out("f");
            break;
        }
        out(",v:'%s'),", t.value.c_str());
    }
    out(">\n");
}

void out(const std::string& fmt, ...)
{
    // Generate printf string
    va_list args;
    va_start(args, fmt);
    size_t size = _vscprintf(fmt.c_str(), args) + 1;
    std::unique_ptr<char> buffer(new char[size]);
    vsnprintf_s(buffer.get(), size, size, fmt.c_str(), args);
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