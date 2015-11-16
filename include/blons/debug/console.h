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

#ifndef BLONSTECH_DEBUG_CONSOLE_H_
#define BLONSTECH_DEBUG_CONSOLE_H_

// Includes
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
// Public Includes
#include <blons/debug/consolevariable.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Functions for interacting with the game console.
////////////////////////////////////////////////////////////////////////////////
namespace console
{
// Since template generation is done at compile time, user needs
// access to function generation code. You probably don't want
// to touch anything in here
#include <blons/debug/consoleinternal.inl.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Prototype for functions called everytime the console prints.
///
/// The input string contains text that is being printed by the console.
////////////////////////////////////////////////////////////////////////////////
typedef std::function<void(const std::string&)> PrintCallback;

////////////////////////////////////////////////////////////////////////////////
/// \brief Send a text command into the console
///
/// Commands are of the format:
///
///     function_name param1 param2 param3
///
/// Valid argument types include:
/// * integers formatted as `12345`, `-12345`
/// * floats formatted as `3.0`, `-.123`
/// * strings formatted as `single_word`, `"multiword string"`
///
/// \param command Command to send
////////////////////////////////////////////////////////////////////////////////
void in(const std::string& command);

////////////////////////////////////////////////////////////////////////////////
/// \brief Prints to the console. Uses printf format syntax.
///
/// \param fmt Format string
/// \param ... Format values
////////////////////////////////////////////////////////////////////////////////
void out(const std::string fmt, ...);

////////////////////////////////////////////////////////////////////////////////
/// \brief Retrieves a global console variable by name. See blons::console for
/// valid return types. If a return type is not defined, a constant pointer to
/// the generic variable is returned instead. Will throw if the variable has not
/// been declared
///
/// \param name Name of the variable to retrieve
/// \tparam T Type to return value as
/// \return Value of the variable
////////////////////////////////////////////////////////////////////////////////
template <typename T>
T var(const std::string& name)
{
    return internal::__var(name)->to<T>();
}

inline const Variable* var(const std::string& name)
{
    return internal::__var(name);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Sets the value of an existing global console variable. Will throw
/// if a type mismatch occurs or if the variable has not been declared
///
/// \param name Name of the variable to modify
/// \param value New value of the variable
////////////////////////////////////////////////////////////////////////////////
template <typename T>
void set_var(const std::string& name, T value)
{
    Variable v(value);
    internal::__set_var(name, v);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Retrieves a list of every function registered to the console
///
/// \return String vector containing functions
////////////////////////////////////////////////////////////////////////////////
const std::vector<std::string> functions();

////////////////////////////////////////////////////////////////////////////////
/// \brief Retrieves an ordered list of every command entered into the console
///
/// \return String vector containing commands
////////////////////////////////////////////////////////////////////////////////
const std::vector<std::string>& history();

////////////////////////////////////////////////////////////////////////////////
/// \brief Register a C++ function to be used as a console command.
///
/// The callback function must return void and can only accept certain types of
/// inputs as parameters. See blons::console for valid inputs.
///
/// If you register a function that is already in use it will either throw if
/// the input parameters are the same, or act as an overloaded function if the
/// inputs are different.
///
/// If you register a function with the same name as an existing variable it
/// will throw.
///
/// \param name The name the function will be called by
/// \param func Function that will be called whenever invoked by the console
////////////////////////////////////////////////////////////////////////////////
template <typename... Args>
inline void RegisterFunction(const std::string& name, std::function<void(Args...)> func)
{
    internal::Function* f = new internal::TemplatedFunction<Args...>(func);
    internal::__registerfunction(name, f);
}

////////////////////////////////////////////////////////////////////////////////
/// \copydoc console::RegisterFunction
////////////////////////////////////////////////////////////////////////////////
inline void RegisterFunction(const std::string& name, std::function<void()> func)
{
    internal::Function* f = new internal::VoidFunction(func);
    internal::__registerfunction(name, f);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Register a global console variable to be used for configuration.
///
/// The variable is statically typed and only supports certain native C++ types.
/// See blons::console for valid inputs.
///
/// If you register a variable that is already in use or has the same name as an
/// existing function it will throw
///
/// \param name Name of the variable
/// \param value Value of the variable
////////////////////////////////////////////////////////////////////////////////
template <typename T>
void RegisterVariable(const std::string& name, T value)
{
    Variable v(value);
    internal::__registervariable(name, v);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Registers a function that will be called whenever the console prints.
///
/// The callback function will be sent a `const std::string&` containing
/// the text that is being printed each call. There can be multiple callbacks
/// stored at a time.
///
/// \param callback Function to be called when the console prints
////////////////////////////////////////////////////////////////////////////////
void RegisterPrintCallback(PrintCallback callback);
} // namespace console
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \namespace blons::console
/// \ingroup debug
///
/// The console allows you to print, register overloadable C++ functions, parse
/// input, and register global console variables for configuration.
///
/// Console variables are statically typed and only compatible certain C++
/// native types:
/// * `int`
/// * `float`
/// * `const char*`
/// * `std::string`
///
/// ### Example:
/// \code
/// // Output
/// blons::console::out("Printing to the console!\n");
///
/// // Printing to stdout
/// blons::console::PrintCallback print = [](const std::string& out)
/// {
///     std::cout << out;
/// }
/// blons::console::RegisterPrintCallback(print);
/// blons::console::out("Printing to stdout!\n");
///
/// // Registering a console function
/// std::function<void(int, int)> add = [](int x, int y)
/// {
///     blons::console::out("Sum is %i\n", x + y);
/// }
/// blons::console::RegisterFunction("add", add);
/// blons::console::in("add 10 20"); // Prints "Sum is 30"
///
/// // Registering a console variable
/// blons::console::RegisterVariable("my_int", 5);
/// blons::console::set_var("my_int", 10);
/// auto my_int = blons::console::var<int>("my_int"); // int with a value of 10
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_DEBUG_CONSOLE_H_