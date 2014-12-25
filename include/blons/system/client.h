#ifndef BLONSTECH_SYSTEM_CLIENT_H_
#define BLONSTECH_SYSTEM_CLIENT_H_

// Strips less used APIs from inclusion
#define WIN32_LEAN_AND_MEAN

// Includes
#include <memory>
#include <Windows.h>
// Public Includes
#include <blons/input/inputtemp.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Creates and handles system windows. Currently a haphazed mess that
/// needs to be rewritten from the ground up
////////////////////////////////////////////////////////////////////////////////
class Client
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Contains the width and height of the window (screen space only) as
    /// well as a HWND (**temporary**)
    ////////////////////////////////////////////////////////////////////////////////
    struct Info
    {
        units::pixel width, height;
        HWND hwnd;
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new window with a predefined width and height of either
    /// 800x600 or whatever the monitor size is depending on some constant global
    /// variable defined in some other header. Uh, that's not great
    ////////////////////////////////////////////////////////////////////////////////
    Client();
    ~Client();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input polling due to nasty intertwining of the Win32 window
    /// and the input is receives. Only temporary. Also does other windowey stuff
    ///
    /// \return True when a quit message has been received
    ////////////////////////////////////////////////////////////////////////////////
    bool Frame();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the screen dimensions and HWND handle of the window
    ///
    /// \return Screen info
    ////////////////////////////////////////////////////////////////////////////////
    Info screen_info() const;

    // TODO: Temporary until Direct & Raw input are setup
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Win32 callback function
    ///
    /// \return Some crazy Win32 macro if we handled it
    ////////////////////////////////////////////////////////////////////////////////
    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the current Input instance of the window. Used for reading
    /// input during each frame
    ///
    /// \return Input instance
    ////////////////////////////////////////////////////////////////////////////////
    Input* input() const;

private:
    // Private functions
    void InitWindow(units::pixel* width, units::pixel* height);

    // Private members
    LPCWSTR app_name_;
    HINSTANCE hinstance_;
    Info screen_info_;

    // TODO: Separate this from client
    std::unique_ptr<Input> input_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Client
/// \ingroup system
///
/// ### Example:
/// \code
/// // There'll be an example after this class is overhauled...
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_SYSTEM_CLIENT_H_