#include "consoleparser.h"

#include <blons/debug/console.h>

namespace
{
using namespace blons::console::internal;
struct ParseState
{
    std::vector<ConsoleArg> args;
    // Valid input always has a function name as the first word
    ConsoleArg current_arg = ConsoleArg{ ConsoleArg::FUNCTION, "" };
    unsigned char current_letter;
    // Records whether we are in the middle of a "quoted string"
    bool open_string = false;
};
} // namespace

namespace blons
{
namespace console
{
void ParseSpace(ParseState* state)
{
    // If we are NOT inside a "quoted string"...
    if (!state->open_string)
    {
        // Make sure we've parsed part of an argument (no double spaces)
        if (state->current_arg.type == ConsoleArg::NONE ||
            state->current_arg.value.length() == 0)
        {
            throw "Unexpected space";
        }
        // Push that argument to the return list
        state->args.push_back(state->current_arg);
        state->current_arg.type = ConsoleArg::NONE;
        state->current_arg.value.clear();
    }
    // If we are inside a "quoted string"...
    else
    {
        state->current_arg.value += ' ';
    }
}

void ParseNumber(ParseState* state)
{
    // Ensure this is the start of a word
    if (state->current_arg.type == ConsoleArg::NONE)
    {
        state->current_arg.type = ConsoleArg::INT;
    }
    // If we aren't at the start of a word, turn this into a string
    else if (state->current_letter == '-' &&
             state->current_arg.type != ConsoleArg::FUNCTION)
    {
        state->current_arg.type = ConsoleArg::STRING;
    }
    state->current_arg.value += state->current_letter;
}

void ParseFloat(ParseState* state)
{
    // If we find a '.' at the start of a word, or in the middle
    // of an integer, then convert to float.
    if (state->current_arg.type == ConsoleArg::INT ||
        state->current_arg.type == ConsoleArg::NONE)
    {
        state->current_arg.type = ConsoleArg::FLOAT;
    }
    state->current_arg.value += state->current_letter;
}

void ParseQuote(ParseState* state)
{
    // Opening quotes are only valid at the start of a word
    if (state->current_arg.type != ConsoleArg::NONE &&
        state->open_string == false)
    {
        throw "Unexpected quote";
    }
    // If this is an opening quote, convert to string
    if (!state->open_string)
    {
        state->current_arg.type = ConsoleArg::STRING;
    }
    // If this is a closing quote, push arg and reset to none
    else
    {
        state->args.push_back(state->current_arg);
        state->current_arg.type = ConsoleArg::NONE;
        state->current_arg.value.clear();
    }
    state->open_string = !state->open_string;
}

void ParseString(ParseState* state)
{
    // Make sure this isnt the first word before we convert
    if (state->current_arg.type != ConsoleArg::FUNCTION)
    {
        state->current_arg.type = ConsoleArg::STRING;
    }
    state->current_arg.value += state->current_letter;
}

void ParseFinish(ParseState* state)
{
    // Make sure all "quoted strings are closed
    if (state->open_string)
    {
        throw "Missing close quote";
    }
    // Check for trailing arguments
    if (state->current_arg.type != ConsoleArg::NONE &&
        state->current_arg.value.length() != 0)
    {
        state->args.push_back(state->current_arg);
    }
}

std::vector<ConsoleArg> ParseCommand(const std::string& command)
{
    ParseState state;
    for (const auto& c : command)
    {
        state.current_letter = c;

        if (c == ' ')
        {
            ParseSpace(&state);
        }
        else if ((c >= '0' && c <= '9') ||
                  c == '-')
        {
            ParseNumber(&state);
        }
        else if (c == '.')
        {
            ParseFloat(&state);
        }
        else if (c == '"')
        {
            ParseQuote(&state);
        }
        // Ascii characters
        else if (c >= 32 && c <= 126)
        {
            ParseString(&state);
        }
        else
        {
            throw "Unexpected character";
        }
    }
    ParseFinish(&state);

    return state.args;
}
} // namespace console
} // namespace blons