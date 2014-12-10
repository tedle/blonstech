#include <debug/console.h>

// Includes
#include <stdarg.h>

namespace
{
using namespace blons::console;
struct ConsoleState
{
    std::vector<PrintCallback> print_callbacks;
} g_state;

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

// TODO: Move parsing functions to separate translation unit
struct ParseState
{
    std::vector<ConsoleArg> args;
    ConsoleArg current_arg = ConsoleArg{ ConsoleArg::FUNCTION, "" };
    unsigned char current_letter;
    bool open_string = false;
};

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

std::vector<ConsoleArg> ParseArgs(const std::string& command)
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
} // namespace

namespace blons
{
namespace console
{
void in(const std::string& command)
{
    out("~ " + command + '\n');

    // Spaghetti parser is go
    std::vector<ConsoleArg> args;
    try
    {
        args = ParseArgs(command);
    }
    catch (const char* error)
    {
        out("%s in: %s\n", error, command.c_str());
    }

    out("<");
    for (const auto& t : args)
    {
        out("(t:");
        switch (t.type)
        {
        case ConsoleArg::NONE:
            out("n");
            break;
        case ConsoleArg::FUNCTION:
            out("c");
            break;
        case ConsoleArg::STRING:
            out("s");
            break;
        case ConsoleArg::INT:
            out("i");
            break;
        case ConsoleArg::FLOAT:
            out("f");
            break;
        }
        out(",v:'%s'),", t.value.c_str());
    }
    out(">\n");
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