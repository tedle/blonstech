#include <blons/debug/console.h>

// Includes
#include <stdarg.h>
#include <unordered_map>
// Local Includes
#include "consoleparser.h"

namespace
{
struct ConsoleState
{
    std::vector<blons::console::PrintCallback> print_callbacks;
    // TODO: Overloaded functions
    std::unordered_map<std::string, std::unique_ptr<blons::console::internal::Function>> functions;
} g_state;
const std::string kErrorPrefix = "* $E33";
const std::string kUserPrefix = "~ $CCE";
} // namespace

namespace blons
{
namespace console
{
using namespace internal;

void internal::__register(const std::string& name, Function* f)
{
    g_state.functions[name] = std::unique_ptr<Function>(f);
}

void in(const std::string& command)
{
    out(kUserPrefix + command + '\n');

    // Spaghetti parser is go
    std::vector<ConsoleArg> args;
    try
    {
        args = ParseCommand(command);
    }
    catch (const char* error)
    {
        out(kErrorPrefix + "%s in: %s\n", error, command.c_str());
        return;
    }

    if (args.size() < 1 || args[0].type != ConsoleArg::FUNCTION)
    {
        out(kErrorPrefix + "Function call requires a name\n");
        return;
    }

    auto func = g_state.functions.find(args[0].value);
    if (func == g_state.functions.end())
    {
        out(kErrorPrefix + "Unknown function \"%s\"\n", args[0].value);
        return;
    }

    // Remove function name from arg list
    args.erase(args.begin());

    auto expected_args = func->second->ArgList();
    if (args.size() != expected_args.size())
    {
        out(kErrorPrefix + "Expected %i arguments, %i given\n", expected_args.size(), args.size());
        return;
    }

    for (int i = 0; i < args.size(); i++)
    {
        if (args[i].type != expected_args[i])
        {
            out(kErrorPrefix + "Wrong type given for argument %i\n", i + 1);
            return;
        }
    }

    func->second->Run(args);

    return;
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