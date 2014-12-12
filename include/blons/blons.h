#ifndef BLONSTECH_BLONS_H_
#define BLONSTECH_BLONS_H_

#include <blons/graphics/graphics.h>
#include <blons/graphics/gui/gui.h>
#include <blons/os/client.h>

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
/// input, windowing, and much more. I'd recommend you don't use this because I
/// don't really know what I'm doing. Good luck!
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
///     auto graphics = std::make_unique<blons::Graphics>(info.width, info.height, info.hwnd);
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