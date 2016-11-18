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

#ifndef BLONSTECH_GRAPHICS_RENDER_CONTEXT_H_
#define BLONSTECH_GRAPHICS_RENDER_CONTEXT_H_

// Includes
#include <blons/graphics/render/renderer.h>
#include <blons/system/client.h>

namespace blons
{
// Forward declarations
class Renderer;

namespace render
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Determines how the system window should be setup for rendering
////////////////////////////////////////////////////////////////////////////////
enum Mode
{
    FULLSCREEN,       ///< GPU based fullscreen rendering
    WINDOW,           ///< Render inside a draggable window
    BORDERLESS_WINDOW ///< Render inside a window fit to the screen, hiding taskbar
};

////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for graphics creation
////////////////////////////////////////////////////////////////////////////////
const Mode kMode = Mode::WINDOW;
////////////////////////////////////////////////////////////////////////////////
/// \brief **Temporary** config option for graphics creation
////////////////////////////////////////////////////////////////////////////////
const bool kEnableVsync = false;

Renderer* context();
void MakeContext(const Client::Info& client);
} // namespace render
} // namespace blons

#endif // BLONSTECH_GRAPHICS_RENDER_CONTEXT_H_
