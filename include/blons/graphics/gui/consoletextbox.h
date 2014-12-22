#ifndef BLONSTECH_GUI_CONSOLETEXTBOX_H_
#define BLONSTECH_GUI_CONSOLETEXTBOX_H_

// Public Includes
#include <blons/graphics/gui/textbox.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Similar to gui::Textbox. Differences include new aesthetics,
/// "always on top" input consumption, Up/Down input history, and console tab
/// completion.
///
/// \copydoc gui::Textbox
////////////////////////////////////////////////////////////////////////////////
class ConsoleTextbox : public Textbox
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Textbox(Box, FontStyle, Manager*, Window*)
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleTextbox(Box pos, Skin::FontStyle style, Manager* parent_manager, Window* parent_window)
        : Textbox(pos, style, parent_manager, parent_window) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls ConsoleTextbox(Box, FontStyle, Manager*, Window*) with a
    /// style of FontStyle::CONSOLE
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleTextbox(Box pos, Manager* parent_manager, Window* parent_window)
        : ConsoleTextbox(pos, Skin::FontStyle::CONSOLE, parent_manager, parent_window) {}
    ~ConsoleTextbox() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context) override;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input logic to deal with typing, history search, and tab
    /// completion.
    ///
    /// \param input Handle to the current input events
    /// \return True as input is always consumed by this class
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;
};
} // namespace gui
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::ConsoleTextbox
/// \ingroup gui
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GUI_CONSOLETEXTBOX_H_