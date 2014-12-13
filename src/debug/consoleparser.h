#ifndef BLONSTECH_DEBUG_CONSOLEPARSER_H_
#define BLONSTECH_DEBUG_CONSOLEPARSER_H_

// Includes
#include <string>
#include <vector>

namespace blons
{
namespace console
{
namespace internal
{
// Forward declarations
struct ConsoleArg;
} // namespace internal

////////////////////////////////////////////////////////////////////////////////
/// \brief Parses console input text into a list of function arguments
///
/// Will throw on failure.
///
/// \param command Text to be parsed
/// \return A list of function arguments. The first argument is always the
/// function name.
////////////////////////////////////////////////////////////////////////////////
std::vector<internal::ConsoleArg> ParseCommand(const std::string& command);
} // namespace console
} // namespace blons

#endif // BLONSTECH_DEBUG_CONSOLEPARSER_H_