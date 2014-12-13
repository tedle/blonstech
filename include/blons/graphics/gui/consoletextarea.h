#ifndef BLONSTECH_GUI_CONSOLETEXTAREA_H_
#define BLONSTECH_GUI_CONSOLETEXTAREA_H_

// Public Includes
#include <blons/graphics/gui/textarea.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Identical to gui::Textarea except for appearance.
///
/// \copydoc gui::Textarea
////////////////////////////////////////////////////////////////////////////////
class ConsoleTextarea : public Textarea
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Textarea(Box, FontStyle, Manager*, Window*)
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleTextarea(Box pos, FontStyle style, Manager* parent_manager, Window* parent_window)
        : Textarea(pos, style, parent_manager, parent_window) {}
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls ConsoleTextarea(Box, FontStyle, Manager*, Window*) with a
    /// style of FontStyle::CONSOLE
    ////////////////////////////////////////////////////////////////////////////////
    ConsoleTextarea(Box pos, Manager* parent_manager, Window* parent_window)
        : ConsoleTextarea(pos, FontStyle::CONSOLE, parent_manager, parent_window) {}
    ~ConsoleTextarea() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager
    ///
    /// \param context Handle to the current rendering context
    ////////////////////////////////////////////////////////////////////////////////
    void Render(RenderContext& context) override;
};
} // namespace gui
} // namespace blons

#endif // BLONSTECH_GUI_CONSOLETEXTAREA_H_
