#include <blons/debug/console.h>

// Includes
#include <stdarg.h>
#include <unordered_map>
// Local Includes
#include "consoleparser.h"

namespace blons
{
namespace console
{
using internal::Function;
using internal::Variable;

namespace
{
typedef std::vector<std::unique_ptr<Function>> FunctionList;

struct ConsoleState
{
    std::vector<PrintCallback> print_callbacks;
    // Stored as a vector to allow function overloading
    std::unordered_map<std::string, FunctionList> functions;
    std::unordered_map<std::string, Variable> variables;
} g_state;
const std::string kErrorPrefix = "* $E33";
const std::string kUserPrefix = "~ $CCE";

void PrintUsage(const std::string& func_name, const FunctionList& func_list)
{
    out("%s%s usage list:\n", kErrorPrefix, func_name);
    for (const auto& f : func_list)
    {
        out("%s  ", kErrorPrefix);
        for (const auto& arg : f->ArgList())
        {
            switch (arg)
            {
            case Variable::INT:
                out("int");
                break;
            case Variable::FLOAT:
                out("float");
                break;
            case Variable::STRING:
                out("str");
                break;
            default:
                // Should never happen
                throw "Unknown argument type";
            }
            out(" ");
        }
        out("\n");
    }
}
} // namespace

// The main Register function is templated and needs to be defined in
// public headers. We use this function to hardcode as much as we can
// into the library.
void internal::__register(const std::string& name, Function* func)
{
    const auto arg_list = func->ArgList();
    auto& func_list = g_state.functions[name];
    // Check if there's already a function with this name and argument list
    for (auto& f : func_list)
    {
        if (f->ArgList() == arg_list)
        {
            f.reset(func);
            return;
        }
    }
    func_list.push_back(std::unique_ptr<Function>(func));
}

const Variable& internal::__var(const std::string& name)
{
    return g_state.variables[name];
}

void in(const std::string& command)
{
    out(kUserPrefix + command + '\n');

    std::vector<Variable> args;
    try
    {
        args = ParseCommand(command);
    }
    catch (const char* error)
    {
        out(kErrorPrefix + "%s in: %s\n", error, command.c_str());
        return;
    }

    if (args.size() < 1 || args[0].type != Variable::FUNCTION)
    {
        out(kErrorPrefix + "Function call requires a name\n");
        return;
    }

    auto func_name = args[0].value;
    auto func_list = g_state.functions.find(func_name);
    if (func_list == g_state.functions.end())
    {
        out(kErrorPrefix + "Unknown function \"%s\"\n", args[0].value);
        return;
    }

    // Remove function name from arg list
    args.erase(args.begin());

    // Find a function matching input arguments and call it
    for (const auto& f : func_list->second)
    {
        const auto expected_args = f->ArgList();
        if (args.size() == expected_args.size())
        {
            for (int i = 0; i < args.size(); i++)
            {
                if (args[i].type != expected_args[i])
                {
                    break;
                }
                if (i == args.size() - 1)
                {
                    f->Run(args);
                    return;
                }
            }
        }
    }
    // Could not find a matching argument set, call fails
    PrintUsage(func_name, func_list->second);
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

    // Send formatted text to every registered print callback
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