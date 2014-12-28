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
    // Stored as a vector to allow function overloading
    std::unordered_map<std::string, FunctionList> functions;
    std::unordered_map<std::string, Variable> variables;
    std::vector<PrintCallback> print_callbacks;
    std::vector<std::string> history;
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

// The main RegisterFunction function is templated and needs to be defined in
// public headers. We use this function to hardcode as much as we can
// into the library.
void internal::__registerfunction(const std::string& name, Function* func)
{
    const auto arg_list = func->ArgList();
    auto& func_list = g_state.functions[name];
    // Check if there's already a function with this name and argument list
    for (auto& f : func_list)
    {
        if (f->ArgList() == arg_list)
        {
            throw "Function already defined";
        }
    }
    func_list.push_back(std::unique_ptr<Function>(func));
}

// The main RegisterVariable function is templated and needs to be defined in
// public headers. We use this function to hardcode as much as we can
// into the library.
void internal::__registervariable(const std::string& name, const Variable& var)
{
    if (g_state.variables.find(name) == g_state.variables.end())
    {
        g_state.variables[name] = var;
    }
    else
    {
        throw "Variable already defined";
    }
}

const Variable& internal::__var(const std::string& name)
{
    auto v = g_state.variables.find(name);
    if (v != g_state.variables.end())
    {
        return v->second;
    }
    else
    {
        throw "Variable not found";
    }
}

void internal::__set_var(const std::string& name, const Variable& value)
{
    auto v = g_state.variables.find(name);
    if (v != g_state.variables.end())
    {
        if (v->second.type() == value.type())
        {
            v->second = value;
        }
        else
        {
            throw "Type mismatch";
        }
    }
    else
    {
        throw "Variable not found";
    }
}

void in(const std::string& command)
{
    out(kUserPrefix + command + '\n');
    if (command.length() == 0)
    {
        return;
    }
    g_state.history.push_back(command);

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

    if (args.size() < 1 || args[0].type() != Variable::FUNCTION)
    {
        out(kErrorPrefix + "Function call requires a name\n");
        return;
    }

    auto func_name = args[0].to<std::string>();
    auto func_list = g_state.functions.find(func_name);
    if (func_list == g_state.functions.end())
    {
        out(kErrorPrefix + "Unknown function \"%s\"\n", func_name.c_str());
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
            // Void functions dont need param checking
            if (args.size() == 0)
            {
                f->Run(args);
                return;
            }

            for (int i = 0; i < args.size(); i++)
            {
                if (args[i].type() != expected_args[i])
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

const std::vector<std::string>& history()
{
    return g_state.history;
}

void RegisterPrintCallback(PrintCallback callback)
{
    g_state.print_callbacks.push_back(callback);
}
} // namespace console
} // namespace blons