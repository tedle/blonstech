#ifndef BLONSTECH_INPUT_INPUTTEMP_H_
#define BLONSTECH_INPUT_INPUTTEMP_H_

// Includes
#include <vector>
// Local Includes
#include "math/units.h"

namespace blons
{
class Input
{
public:
    enum KeyCode
    {
        BAD                  = 0x00,

        PRINTABLE_START      = 0x01,
        CHAR_0               = 0x01,
        CHAR_1               = 0x02,
        CHAR_2               = 0x03,
        CHAR_3               = 0x04,
        CHAR_4               = 0x05,
        CHAR_5               = 0x06,
        CHAR_6               = 0x07,
        CHAR_7               = 0x08,
        CHAR_8               = 0x09,
        CHAR_9               = 0x0A,

        CHAR_A               = 0x0B,
        CHAR_B               = 0x0C,
        CHAR_C               = 0x0D,
        CHAR_D               = 0x0E,
        CHAR_E               = 0x0F,
        CHAR_F               = 0x10,
        CHAR_G               = 0x11,
        CHAR_H               = 0x12,
        CHAR_I               = 0x13,
        CHAR_J               = 0x14,
        CHAR_K               = 0x15,
        CHAR_L               = 0x16,
        CHAR_M               = 0x17,
        CHAR_N               = 0x18,
        CHAR_O               = 0x19,
        CHAR_P               = 0x1A,
        CHAR_Q               = 0x1B,
        CHAR_R               = 0x1C,
        CHAR_S               = 0x1D,
        CHAR_T               = 0x1E,
        CHAR_U               = 0x1F,
        CHAR_V               = 0x20,
        CHAR_W               = 0x21,
        CHAR_X               = 0x22,
        CHAR_Y               = 0x23,
        CHAR_Z               = 0x24,

        SYMBOL_START         = 0x25,
        SYMBOL_LEFT_BRACKET  = 0x25,
        SYMBOL_RIGHT_BRACKET = 0x26,
        SYMBOL_GRAVE_ACCENT  = 0x27,
        SYMBOL_BACKSLASH     = 0x28,
        SYMBOL_SEMICOLON     = 0x29,
        SYMBOL_QUOTE         = 0x2A,
        SYMBOL_COMMA         = 0x2B,
        SYMBOL_PERIOD        = 0x2C,
        SYMBOL_SLASH         = 0x2D,
        SYMBOL_MINUS         = 0x2E,
        SYMBOL_EQUALS        = 0x2F,
        SYMBOL_END           = 0x2F,

        SPACE                = 0x30,
        PRINTABLE_END        = 0x30,

        SHIFT                = 0x31,
        CONTROL              = 0x32,
        ALT                  = 0x33,
        BACKSPACE            = 0x34,
        // Shoutouts to a certain company that loves #define!
        DEL                  = 0x35,
        ESCAPE               = 0x36,
        RETURN               = 0x37,

        LEFT                 = 0x38,
        RIGHT                = 0x39,
        UP                   = 0x3A,
        DOWN                 = 0x3B
    };

    struct Event
    {
        enum Type
        {
            MOUSE_DOWN,
            MOUSE_UP,
            MOUSE_MOVE_X,
            MOUSE_MOVE_Y,
            KEY_DOWN,
            KEY_UP
        } type;

        int value;

        Event(Type t, int v) : type(t), value(v) {}
    };

    // Helper class for looping thru event queues
    struct Modifiers
    {
        void Update(Event e);

        bool alt;
        bool ctrl;
        bool shift;
    };

public:
    Input();
    ~Input() {}

    void KeyDown(KeyCode key_code);
    void KeyUp(KeyCode key_code);

    bool IsKeyDown(KeyCode key_code) const;

    void MouseDown(unsigned int);
    void MouseUp(unsigned int);

    bool IsMouseDown(unsigned int) const;

    void MouseMove(int, int);

    bool Frame();

    bool IsPrintable(KeyCode key_code) const;
    unsigned char ToAscii(KeyCode key_code) const;
    unsigned char ToAscii(KeyCode key_code, bool shift) const;

    Modifiers modifiers() const;

    units::pixel mouse_x() const;
    units::pixel mouse_y() const;
    units::pixel mouse_delta_x() const;
    units::pixel mouse_delta_y() const;
    const std::vector<Event>& Input::event_queue() const;

private:
    bool keys_[256];
    units::pixel mouse_x_, mouse_y_, old_mouse_x_, old_mouse_y_, delta_mouse_x_, delta_mouse_y_;
    bool buttons_[5];

    std::vector<Event> event_queue_;
    std::vector<Event> event_queue_buffer_;
};
} // namespace blons

#endif