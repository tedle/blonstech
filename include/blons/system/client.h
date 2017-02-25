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

#ifndef BLONSTECH_SYSTEM_CLIENT_H_
#define BLONSTECH_SYSTEM_CLIENT_H_

// Strips less used APIs from inclusion
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Includes
#include <memory>
// TODO: Prevent this from being in client.h somehow?
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
        ////////////////////////////////////////////////////////////////////////////////
        //@{
        /// Screen dimensions in pixels
        units::pixel width, height;
        //@}
        ////////////////////////////////////////////////////////////////////////////////
        /// OS window handle
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