////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
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

#ifndef BLONSTECH_GRAPHICS_GUI_GUI_H_
#define BLONSTECH_GRAPHICS_GUI_GUI_H_

// Public Includes
#include <blons/graphics/gui/manager.h>

////////////////////////////////////////////////////////////////////////////////
/// \defgroup gui GUI
/// \ingroup graphics
/// \brief UI logic and rendering
///
/// Provides a blons::gui::Manager class for easy creation and management of UI
/// elements. An instance of blons::gui::Manager is automatically created and
/// retrievable from any active blons::Graphics manager class.
///
/// Also provides an abstract class blons::gui::Control for deriving new types
/// of UI elements.
///
/// ### Example:
/// \code
/// // Retrieving the gui::Manager
/// auto gui = graphics->gui();
///
/// // Creating a Window
/// auto window = gui->MakeWindow("some id", 0, 0, 300, 300, "Window title!");
///
/// // Retrieving the Window later
/// window = gui->window("some id");
///
/// // Adding a button to the window
/// window->MakeButton(10, 150, 120, 40, "Hey!");
///
/// // Render loop
/// while (true)
/// {
///     // Input logic
///     gui->Update(*client->input());
///
///     // Rendering the UI (and everything else)
///     graphics->Render();
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \namespace blons::gui
/// \brief Encapsulates anything related to UI logic
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_GUI_H_