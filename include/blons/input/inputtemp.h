#ifndef BLONSTECH_INPUT_INPUTTEMP_H_
#define BLONSTECH_INPUT_INPUTTEMP_H_

// Includes
#include <vector>
// Public Includes
#include <blons/math/units.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Handles input polling and storage of events
////////////////////////////////////////////////////////////////////////////////
class Input
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Translates keyboard keys to a hardcoded value
    ////////////////////////////////////////////////////////////////////////////////
    enum KeyCode
    {
        BAD                  = 0x00, ///< Error

        PRINTABLE_START      = 0x01, ///< Start of the range of printable keyboard keys
        CHAR_0               = 0x01, ///< 0
        CHAR_1               = 0x02, ///< 1
        CHAR_2               = 0x03, ///< 2
        CHAR_3               = 0x04, ///< 3
        CHAR_4               = 0x05, ///< 4
        CHAR_5               = 0x06, ///< 5
        CHAR_6               = 0x07, ///< 6
        CHAR_7               = 0x08, ///< 7
        CHAR_8               = 0x09, ///< 8
        CHAR_9               = 0x0A, ///< 9

        CHAR_A               = 0x0B, ///< A
        CHAR_B               = 0x0C, ///< B
        CHAR_C               = 0x0D, ///< C
        CHAR_D               = 0x0E, ///< D
        CHAR_E               = 0x0F, ///< E
        CHAR_F               = 0x10, ///< F
        CHAR_G               = 0x11, ///< G
        CHAR_H               = 0x12, ///< H
        CHAR_I               = 0x13, ///< I
        CHAR_J               = 0x14, ///< J
        CHAR_K               = 0x15, ///< K
        CHAR_L               = 0x16, ///< L
        CHAR_M               = 0x17, ///< M
        CHAR_N               = 0x18, ///< N
        CHAR_O               = 0x19, ///< O
        CHAR_P               = 0x1A, ///< P
        CHAR_Q               = 0x1B, ///< Q
        CHAR_R               = 0x1C, ///< R
        CHAR_S               = 0x1D, ///< S
        CHAR_T               = 0x1E, ///< T
        CHAR_U               = 0x1F, ///< U
        CHAR_V               = 0x20, ///< V
        CHAR_W               = 0x21, ///< W
        CHAR_X               = 0x22, ///< X
        CHAR_Y               = 0x23, ///< Y
        CHAR_Z               = 0x24, ///< Z

        SYMBOL_START         = 0x25, ///< Start of the range of printable symbolic keyboard keys
        SYMBOL_LEFT_BRACKET  = 0x25, ///< [
        SYMBOL_RIGHT_BRACKET = 0x26, ///< ]
        SYMBOL_GRAVE_ACCENT  = 0x27, ///< `
        SYMBOL_BACKSLASH     = 0x28, ///< \ 
        SYMBOL_SEMICOLON     = 0x29, ///< ;
        SYMBOL_QUOTE         = 0x2A, ///< '
        SYMBOL_COMMA         = 0x2B, ///< ,
        SYMBOL_PERIOD        = 0x2C, ///< Period symbol
        SYMBOL_SLASH         = 0x2D, ///< /
        SYMBOL_MINUS         = 0x2E, ///< Minus symbol
        SYMBOL_EQUALS        = 0x2F, ///< =
        SYMBOL_END           = 0x2F, ///< End of the range of printable symbolic keyboard keys

        SPACE                = 0x30, ///< Spacebar
        PRINTABLE_END        = 0x30, ///< End of the range of printable keyboard keys

        SHIFT                = 0x31, ///< Shift modifier
        CONTROL              = 0x32, ///< Control modifier
        ALT                  = 0x33, ///< Alt modifier
        BACKSPACE            = 0x34, ///< Backspace key

        DEL                  = 0x35, ///< Delete key (shoutouts to a certain company that loves macros!)
        ESCAPE               = 0x36, ///< Escape key
        RETURN               = 0x37, ///< Return (enter) key

        PG_UP                = 0x38, ///< Page up
        PG_DOWN              = 0x39, ///< Page down

        LEFT                 = 0x3A, ///< Left arrow key
        RIGHT                = 0x3B, ///< Right arrow key
        UP                   = 0x3C, ///< Up arrow key
        DOWN                 = 0x3D  ///< Down arrow key
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Describes and input event
    ////////////////////////////////////////////////////////////////////////////////
    struct Event
    {
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Type of input event that occured
        ////////////////////////////////////////////////////////////////////////////////
        enum Type
        {
            MOUSE_DOWN,   ///< Mouse button was pressed
            MOUSE_UP,     ///< Mouse button was released
            MOUSE_MOVE_X, ///< Mouse moved along X axis
            MOUSE_MOVE_Y, ///< Mouse moved along Y axis
            MOUSE_SCROLL, ///< Mouse wheel was scrolled
            KEY_DOWN,     ///< Keyboard key was pressed
            KEY_UP        ///< Keyboard key was released
        } type; ///< Type of input event that occured

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Value of the input event
        ///
        /// %Event Type          | Value
        /// -----------          | -----
        /// KEY_DOWN, KEY_UP     | Input::KeyCode describing key pressed
        /// MOUSE_DOWN, MOUSE_UP | Number corresponding to button (**temporary**)
        /// MOUSE_MOVE_X         | Number of pixels moved along the X axis
        /// MOUSE_MOVE_Y         | Number of pixels moved along the Y axis
        /// MOUSE_SCROLL         | Number of scroll clicks, positive for scroll up
        ////////////////////////////////////////////////////////////////////////////////
        int value;

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Initializes a new Event with the specified type and value
        ///
        /// \param t Event::Type of event
        /// \param v Value of the event
        ////////////////////////////////////////////////////////////////////////////////
        Event(Type t, int v) : type(t), value(v) {}
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Helper class for tracking and containing the state of modifier keys
    ////////////////////////////////////////////////////////////////////////////////
    struct Modifiers
    {
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief Helper class for tracking and containing the state of modifier keys
        ///
        /// \param e %Event to parse for pressed modifier keys
        ////////////////////////////////////////////////////////////////////////////////
        void Update(Event e);

        ////////////////////////////////////////////////////////////////////////////////
        /// \brief True if the alt key is currently down
        ////////////////////////////////////////////////////////////////////////////////
        bool alt;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief True if the control key is currently down
        ////////////////////////////////////////////////////////////////////////////////
        bool ctrl;
        ////////////////////////////////////////////////////////////////////////////////
        /// \brief True if the shift key is currently down
        ////////////////////////////////////////////////////////////////////////////////
        bool shift;
    };

public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes a new Input class with all state set to zero
    ////////////////////////////////////////////////////////////////////////////////
    Input();
    ~Input() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Simulates a KEY_DOWN event for the specified key
    ///
    /// \param key_code Key to be pressed down
    ////////////////////////////////////////////////////////////////////////////////
    void KeyDown(KeyCode key_code);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Simulates a KEY_UP event for the specified key
    ///
    /// \param key_code Key to be released
    ////////////////////////////////////////////////////////////////////////////////
    void KeyUp(KeyCode key_code);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Checks whether the specified key is currently pressed down
    ///
    /// \param key_code Key to be checked
    /// \return True if the key is currently pressed
    ////////////////////////////////////////////////////////////////////////////////
    bool IsKeyDown(KeyCode key_code) const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Simulates a MOUSE_DOWN event for the specified button
    ///
    /// \param button Button to be pressed down
    ////////////////////////////////////////////////////////////////////////////////
    void MouseDown(unsigned int button);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Simulates a MOUSE_UP event for the specified button
    ///
    /// \param button Button to be released
    ////////////////////////////////////////////////////////////////////////////////
    void MouseUp(unsigned int button);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Checks whether the specified mouse button is currently pressed down
    ///
    /// \param button Mouse button to be checked
    /// \return True if the button is currently pressed
    ////////////////////////////////////////////////////////////////////////////////
    bool IsMouseDown(unsigned int button) const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Simulates moving the mouse cursor a specified amount of pixels
    ///
    /// \param x Pixels to move along the X axis (positive right, negative left)
    /// \param y Pixels to move along the Y axis (positive down, negative up)
    ////////////////////////////////////////////////////////////////////////////////
    void MouseMove(int x, int y);
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Simulates scrolling the mouse wheel a specified number of clicks
    ///
    /// \param delta Number of clicks to scroll (positive up, negative down)
    ////////////////////////////////////////////////////////////////////////////////
    void MouseScroll(int delta);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs input processing for the frame, prepping the event queue.
    /// Will toss out all old input data
    ////////////////////////////////////////////////////////////////////////////////
    void Frame();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Checks whether a specified key code can be printed on screen
    ///
    /// \param key_code Key code to check
    /// \return True if the character is printable
    ////////////////////////////////////////////////////////////////////////////////
    bool IsPrintable(KeyCode key_code) const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the ASCII value of a given key code
    ///
    /// \param key_code Key code to translate
    /// \param shift True if the shift key is held and ASCII value should be
    /// capitalized
    /// \return ASCII character, or 0 on failure
    ////////////////////////////////////////////////////////////////////////////////
    unsigned char ToAscii(KeyCode key_code, bool shift) const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Calls ToAscii(KeyCode, bool) with a default shift value of false
    ////////////////////////////////////////////////////////////////////////////////
    unsigned char ToAscii(KeyCode key_code) const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a Modifiers struct describing which modifier keys are
    /// currently pressed down
    ///
    /// \return Modifiers struct
    ////////////////////////////////////////////////////////////////////////////////
    Modifiers modifiers() const;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the mouse position along the X axis on screen in pixels
    ///
    /// \return Mouse's X position in pixels
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel mouse_x() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the mouse position along the Y axis on screen in pixels
    ///
    /// \return Mouse's Y position in pixels
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel mouse_y() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the number of pixels mouse moved along the X axis between
    /// the last 2 calls to Input::Frame
    ///
    /// \return Mouse's X movement in pixels (negative for left, positive for right)
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel mouse_delta_x() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the number of pixels mouse moved along the Y axis between
    /// the last 2 calls to Input::Frame
    ///
    /// \return Mouse's Y movement in pixels (negative for up, positive for down)
    ////////////////////////////////////////////////////////////////////////////////
    units::pixel mouse_delta_y() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the number of scroll wheel clicks between the last 2 calls
    /// to Input::Frame
    ///
    /// \return Number of clicks moved (negative for down, positive for up)
    ////////////////////////////////////////////////////////////////////////////////
    int mouse_scroll_delta() const;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a vector list of all input events that happened between
    /// the last 2 calls to Input::Frame in the order that they occured
    ///
    /// \return List of events
    ////////////////////////////////////////////////////////////////////////////////
    const std::vector<Event>& Input::event_queue() const;

private:
    bool keys_[256];
    units::pixel mouse_x_, mouse_y_, old_mouse_x_, old_mouse_y_, delta_mouse_x_, delta_mouse_y_;
    int mouse_scroll_delta_, mouse_scroll_delta_old_;
    bool buttons_[5];

    std::vector<Event> event_queue_;
    std::vector<Event> event_queue_buffer_;
};
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::Input
/// \ingroup input
///
/// Provides access either through the query of an input's state after polling
/// or through an ordered list of input events that occured between the last 2
/// polling calls. The input list is better suited when you need to know all of
/// the inputs that occured since you last checked, whereas querying works best
/// when you only need to know the state of input at the end of a frame
///
/// ### Example:
/// \code
/// // Create a new window with input handling
/// auto client = std::make_unique<blons::Client>();
///
/// bool quit = false;
/// // Main input loop
/// while (!quit)
/// {
///     // Update input
///     quit = client->Frame();
///
///     // Get input class
///     auto input = client->input();
///
///     // Can check for key presses either through query
///     if (input->IsKeyDown(input.RETURN))
///     {
///         blons::log::Debug("Return key was pressed!\n");
///     }
///
///     // Or through an ordered input list
///     for (const auto& e : input->event_queue())
///     {
///         if (e.type == e.KEY_DOWN &&
///             e.value == input.RETURN)
///         {
///             blons::log::Debug("Return key was pressed!\n");
///         }
///     }
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_INPUT_INPUTTEMP_H_