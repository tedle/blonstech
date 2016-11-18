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

#ifndef BLONSTECH_BLONS_H_
#define BLONSTECH_BLONS_H_

#include <blons/debug.h>
#include <blons/graphics.h>
#include <blons/input.h>
#include <blons/math.h>
#include <blons/system.h>

////////////////////////////////////////////////////////////////////////////////
/// \namespace blons
///
/// \brief The main namespace that encapsulates anything defined by blonstech.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \mainpage
///
/// \section intro Welcome, gamer!
/// **blonstech** is a C++ game engine providing abstractions for rendering, UI,
/// input, windowing, and much more, all inside of a modular include system.
/// I'd recommend you don't use this because I don't really know what I'm doing.
/// Good luck!
///
/// \section example Example
/// Here's a simple blonstech program that opens a window and draws a sprite.
/// \code
/// #include <blons/blons.h>
///
/// int main(int argc, char** argv)
/// {
///     auto client = std::make_unique<blons::Client>();
///     auto info = client->screen_info();
///     blons::render::MakeContext(info);
///     auto graphics = std::make_unique<blons::Graphics>(info);
///
///     auto sprite = graphics->MakeSprite("image.png");
///     sprite->set_pos(0, 0, info.width, info.height);
///
///     bool quit = false;
///     while (!quit)
///     {
///         quit = client->Frame();
///         graphics->Render();
///     }
///
///     return 0;
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_BLONS_H_