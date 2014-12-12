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

std::vector<internal::ConsoleArg> ParseCommand(const std::string& command);
} // namespace console
} // namespace blons

#endif // BLONSTECH_DEBUG_CONSOLEPARSER_H_