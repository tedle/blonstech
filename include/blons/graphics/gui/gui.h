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
/// \namespace gui
/// \brief Encapsulates anything related to UI logic
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_GUI_H_