#include "consoleparser.h"

namespace
{
using namespace blons::console;
struct ParseState
{
    std::vector<ConsoleArg> args;
    ConsoleArg current_arg = ConsoleArg{ ConsoleArg::FUNCTION, "" };
    unsigned char current_letter;
    bool open_string = false;
};
} // namespace

namespace blons
{
namespace console
{
void ParseSpace(ParseState* state)
{
    if (!state->open_string)
    {
        if (state->current_arg.type == ConsoleArg::NONE ||
            state->current_arg.value.length() == 0)
        {
            throw "Unexpected space";
        }
        state->args.push_back(state->current_arg);
        state->current_arg.type = ConsoleArg::NONE;
        state->current_arg.value.clear();
    }
    else
    {
        state->current_arg.value += ' ';
    }
}

void ParseNumber(ParseState* state)
{
    if (state->current_arg.type == ConsoleArg::NONE)
    {
        state->current_arg.type = ConsoleArg::INT;
    }
    else if (state->current_letter == '-' &&
             state->current_arg.type != ConsoleArg::FUNCTION)
    {
        state->current_arg.type = ConsoleArg::STRING;
    }
    state->current_arg.value += state->current_letter;
}

void ParseFloat(ParseState* state)
{
    if (state->current_arg.type == ConsoleArg::INT)
    {
        state->current_arg.type = ConsoleArg::FLOAT;
    }
    state->current_arg.value += state->current_letter;
}

void ParseQuote(ParseState* state)
{
    if (state->current_arg.type != ConsoleArg::NONE &&
        state->open_string == false)
    {
        throw "Unexpected quote";
    }
    if (!state->open_string)
    {
        state->current_arg.type = ConsoleArg::STRING;
    }
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
    if (state->current_arg.type != ConsoleArg::FUNCTION)
    {
        state->current_arg.type = ConsoleArg::STRING;
    }
    state->current_arg.value += state->current_letter;
}

void ParseFinish(ParseState* state)
{
    if (state->open_string)
    {
        throw "Missing close quote";
    }
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