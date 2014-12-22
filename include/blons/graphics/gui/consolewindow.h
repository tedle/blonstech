#ifndef BLONSTECH_GRAPHICS_GUI_CONSOLEWINDOW_H_
#define BLONSTECH_GRAPHICS_GUI_CONSOLEWINDOW_H_

// Public Includes
#include <blons/graphics/gui/control.h>
#include <blons/graphics/gui/window.h>
#include <blons/graphics/gui/consoletextarea.h>
#include <blons/graphics/gui/consoletextbox.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Similar to gui::Window. Differences include new aesthetics, an
/// animated hide/show sliding effect, and the automatic insertion of a
/// gui::ConsoleTextarea and gui::ConsoleTextbox during construction.
///
/// \copydoc gui::Window
////////////////////////////////////////////////////////////////////////////////
class ConsoleWindow : public Window
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Window(std::string, Box, std::string, WindowType, Manager*, Window*)
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleWindow(std::string id, Box pos, std::string caption, Type type, Manager* parent_manager);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls
    /// ConsoleWindow(std::string, Box, std::string, WindowType, Manager*)
    /// with an empty caption
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleWindow(std::string id, Box pos, Type type, Manager* parent_manager)
        : ConsoleWindow(id, pos, "", type, parent_manager) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls
    /// ConsoleWindow(std::string, Box, std::string, WindowType, Manager*)
    /// with a style of WindowType::INVISIBLE
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleWindow(std::string id, Box pos, std::string caption, Manager* parent_manager)
        : ConsoleWindow(id, pos, caption, Type::INVISIBLE, parent_manager) {}
    ~ConsoleWindow() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context) override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs no input logic of its own, simply forwards input to child
    /// Control%s.
    ///
    /// \param input Handle to the current input events
    /// \return True if input is consumed by Control%s
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Slides the ConsoleWindow off the top of the screen over the course of
    /// 300ms. hidden() will not return true until this animation is complete.
    ////////////////////////////////////////////////////////////////////////////////
    void hide() override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Slides the ConsoleWindow down from the top of the screen over the
    /// course of 300ms.
    ////////////////////////////////////////////////////////////////////////////////
    void show() override;

private:
    Animation slide_;
    // TODO: Get rid of hacky state recording
    bool hiding_;
};
} // namepsace GUI
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::ConsoleWindow
/// \ingroup gui
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_CONSOLEWINDOW_H_
