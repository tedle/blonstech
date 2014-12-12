#ifndef BLONSTECH_DEBUG_CONSOLE_H_
#define BLONSTECH_DEBUG_CONSOLE_H_

// Includes
#include <functional>
#include <memory>
#include <vector>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Functions for interacting with the game console.
////////////////////////////////////////////////////////////////////////////////
namespace console
{
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
void out(const std::string& fmt, ...);

////////////////////////////////////////////////////////////////////////////////
/// \brief Register a C++ function to be used as a console command.
///
/// The callback function must return void and can only accept certain types of
/// inputs as parameters.
///
/// Valid inputs are:
/// * const char*
/// * float
/// * int
/// * std::string
///
/// If you register a function that is already in use it will either replace
/// the current function if the input parameters are the same, or act as an
/// overloaded function if the inputs are different.
///
/// \param name The name the function will be called by
/// \param func Function that will be called whenever invoked by the console
////////////////////////////////////////////////////////////////////////////////
template <typename... Args>
void Register(const std::string& name, std::function<void(Args...)> func)
{
    internal::Function* f = new internal::TemplatedFunction<Args...>(func);
    internal::__register(name, f);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Register a function that will be called whenever the console prints.
///
/// The callback function will be sent a `const std::string&` containing
/// the text that is being printed each call. There can be multiple callbacks
/// stored at a time.
///
/// \param callback Function to be called when the console prints
////////////////////////////////////////////////////////////////////////////////
void RegisterPrintCallback(PrintCallback callback);

// Since template generation is done at compile time, user needs
// access to function generation code. You probably don't want
// to touch anything in here
#include <blons/debug/consolefunction.inl.h>
} // namespace console
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \namespace blons::console
///
/// The console allows you to print, register C++ functions, and parse input
/// to call registered functions. Also supports function overloading.
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
/// blons::console::Register("add", add);
/// blons::console::in("add 10 20"); // Prints "Sum is 30"
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_DEBUG_CONSOLE_H_