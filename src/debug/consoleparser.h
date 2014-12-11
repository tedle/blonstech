#ifndef BLONSTECH_DEBUG_CONSOLEPARSER_H_
#define BLONSTECH_DEBUG_CONSOLEPARSER_H_

// Includes
#include <string>
#include <vector>

namespace blons
{
namespace console
{
struct ConsoleArg
{
    enum ValueType
    {
        NONE,
        FUNCTION,
        STRING,
        INT,
        FLOAT
    } type;
    std::string value;
};

std::vector<ConsoleArg> ParseCommand(const std::string& command);
} // namespace console
} // namespace blons
#endif