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
struct Variable;
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
std::vector<internal::Variable> ParseCommand(const std::string& command);
} // namespace console
} // namespace blons

#endif // BLONSTECH_DEBUG_CONSOLEPARSER_H_