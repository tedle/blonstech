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

#include "consoleparser.h"

#include <blons/debug/console.h>

namespace blons
{
namespace console
{
namespace
{
struct ParseState
{
    std::vector<Variable> args;
    struct
    {
        Variable::Type type;
        std::string value;
    } current_arg;
    unsigned char current_letter;
    // Records whether we are in the middle of a "quoted string"
    bool open_string = false;
};
} // namespace

void ParseSpace(ParseState* state)
{
    // If we are NOT inside a "quoted string"...
    if (!state->open_string)
    {
        // Make sure we've parsed part of an argument (no double spaces)
        if (state->current_arg.type == Variable::NONE ||
            state->current_arg.value.length() == 0)
        {
            throw "Unexpected space";
        }
        // Push that argument to the return list
        state->args.push_back(Variable(state->current_arg.type, state->current_arg.value));
        state->current_arg.type = Variable::NONE;
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
    if (state->current_arg.type == Variable::NONE)
    {
        state->current_arg.type = Variable::INT;
    }
    // If we aren't at the start of a word, turn this into a string
    else if (state->current_letter == '-' &&
             state->current_arg.type != Variable::FUNCTION)
    {
        state->current_arg.type = Variable::STRING;
    }
    state->current_arg.value += state->current_letter;
}

void ParseFloat(ParseState* state)
{
    // If we find a '.' at the start of a word, or in the middle
    // of an integer, then convert to float.
    if (state->current_arg.type == Variable::INT ||
        state->current_arg.type == Variable::NONE)
    {
        state->current_arg.type = Variable::FLOAT;
    }
    state->current_arg.value += state->current_letter;
}

void ParseQuote(ParseState* state)
{
    // Opening quotes are only valid at the start of a word
    if (state->current_arg.type != Variable::NONE &&
        state->open_string == false)
    {
        throw "Unexpected quote";
    }
    // If this is an opening quote, convert to string
    if (!state->open_string)
    {
        state->current_arg.type = Variable::STRING;
    }
    // If this is a closing quote, push arg and reset to none
    else
    {
        state->args.push_back(Variable(state->current_arg.type, state->current_arg.value));
        state->current_arg.type = Variable::NONE;
        state->current_arg.value.clear();
    }
    state->open_string = !state->open_string;
}

void ParseString(ParseState* state)
{
    // Make sure this isnt the first word before we convert
    if (state->current_arg.type != Variable::FUNCTION)
    {
        state->current_arg.type = Variable::STRING;
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
    if (state->current_arg.type != Variable::NONE &&
        state->current_arg.value.length() != 0)
    {
        state->args.push_back(Variable(state->current_arg.type, state->current_arg.value));
    }
}

std::vector<Variable> ParseCommand(const std::string& command)
{
    ParseState state;
    // Valid input always has a function name as the first word
    state.current_arg.type = Variable::FUNCTION;
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