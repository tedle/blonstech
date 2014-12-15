#include <blons/input/inputtemp.h>

// Includes
#include <unordered_map>

namespace blons
{
namespace
{
static const std::unordered_map<Input::KeyCode, std::pair<unsigned char, unsigned char>> kAsciiMap =
{
    { Input::CHAR_0, { '0', ')' } },
    { Input::CHAR_1, { '1', '!' } },
    { Input::CHAR_2, { '2', '@' } },
    { Input::CHAR_3, { '3', '#' } },
    { Input::CHAR_4, { '4', '$' } },
    { Input::CHAR_5, { '5', '%' } },
    { Input::CHAR_6, { '6', '^' } },
    { Input::CHAR_7, { '7', '&' } },
    { Input::CHAR_8, { '8', '*' } },
    { Input::CHAR_9, { '9', '(' } },

    { Input::CHAR_A, { 'a', 'A' } },
    { Input::CHAR_B, { 'b', 'B' } },
    { Input::CHAR_C, { 'c', 'C' } },
    { Input::CHAR_D, { 'd', 'D' } },
    { Input::CHAR_E, { 'e', 'E' } },
    { Input::CHAR_F, { 'f', 'F' } },
    { Input::CHAR_G, { 'g', 'G' } },
    { Input::CHAR_H, { 'h', 'H' } },
    { Input::CHAR_I, { 'i', 'I' } },
    { Input::CHAR_J, { 'j', 'J' } },
    { Input::CHAR_K, { 'k', 'K' } },
    { Input::CHAR_L, { 'l', 'L' } },
    { Input::CHAR_M, { 'm', 'M' } },
    { Input::CHAR_N, { 'n', 'N' } },
    { Input::CHAR_O, { 'o', 'O' } },
    { Input::CHAR_P, { 'p', 'P' } },
    { Input::CHAR_Q, { 'q', 'Q' } },
    { Input::CHAR_R, { 'r', 'R' } },
    { Input::CHAR_S, { 's', 'S' } },
    { Input::CHAR_T, { 't', 'T' } },
    { Input::CHAR_U, { 'u', 'U' } },
    { Input::CHAR_V, { 'v', 'V' } },
    { Input::CHAR_W, { 'w', 'W' } },
    { Input::CHAR_X, { 'x', 'X' } },
    { Input::CHAR_Y, { 'y', 'Y' } },
    { Input::CHAR_Z, { 'z', 'Z' } },

    { Input::SYMBOL_LEFT_BRACKET,  { '[', '{' } },
    { Input::SYMBOL_RIGHT_BRACKET, { ']', '}' } },
    { Input::SYMBOL_GRAVE_ACCENT,  { '`', '~' } },
    { Input::SYMBOL_BACKSLASH,     { '\\','|' } },
    { Input::SYMBOL_SEMICOLON,     { ';', ':' } },
    { Input::SYMBOL_QUOTE,         { '\'','"' } },
    { Input::SYMBOL_COMMA,         { ',', '<' } },
    { Input::SYMBOL_PERIOD,        { '.', '>' } },
    { Input::SYMBOL_SLASH,         { '/', '?' } },
    { Input::SYMBOL_MINUS,         { '-', '_' } },
    { Input::SYMBOL_EQUALS,        { '=', '+' } },

    { Input::SPACE, { ' ', ' ' } }
};
} // namespace

Input::Input()
{
    for (int i=0; i<256; i++)
        keys_[i] = false;
    for (int i=0; i<2; i++)
        buttons_[i] = false;

    mouse_x_ = mouse_y_ = old_mouse_x_ = old_mouse_y_ = delta_mouse_x_ = delta_mouse_y_ = 0;
    mouse_scroll_delta_ = 0;
}

void Input::KeyDown(KeyCode key_code)
{
    if (!IsKeyDown(key_code))
    {
        event_queue_buffer_.push_back(Event(Event::KEY_DOWN, key_code));
    }
    keys_[key_code] = true;
}

void Input::KeyUp(KeyCode key_code)
{
    if (IsKeyDown(key_code))
    {
        event_queue_buffer_.push_back(Event(Event::KEY_UP, key_code));
    }
    keys_[key_code] = false;
}

bool Input::IsKeyDown(KeyCode key_code) const
{
    return keys_[key_code];
}

void Input::MouseDown(unsigned int vk)
{
    if (!IsMouseDown(vk))
    {
        event_queue_buffer_.push_back(Event(Event::MOUSE_DOWN, vk));
    }
    buttons_[vk] = true;
    return;
}

void Input::MouseUp(unsigned int vk)
{
    if (IsMouseDown(vk))
    {
        event_queue_buffer_.push_back(Event(Event::MOUSE_UP, vk));
    }
    buttons_[vk] = false;
    return;
}

bool Input::IsMouseDown(unsigned int vk) const
{
    return buttons_[vk];
}

void Input::MouseMove(int x, int y)
{
    mouse_x_ = x;
    mouse_y_ = y;
}

void Input::MouseScroll(int delta)
{
    event_queue_buffer_.push_back(Event(Event::MOUSE_SCROLL, delta));
    mouse_scroll_delta_ += delta;
}

bool Input::IsPrintable(KeyCode key_code) const
{
    return (key_code >= KeyCode::PRINTABLE_START && key_code <= KeyCode::PRINTABLE_END);
}

unsigned char Input::ToAscii(KeyCode key_code) const
{
    return ToAscii(key_code, false);
}

unsigned char Input::ToAscii(KeyCode key_code, bool shift) const
{
    try
    {
        if (shift)
        {
            return kAsciiMap.at(key_code).second;
        }
        else
        {
            return kAsciiMap.at(key_code).first;
        }
    }
    catch (...)
    {
        return 0;
    }
}

Input::Modifiers Input::modifiers() const
{
    Modifiers mods;
    mods.alt = IsKeyDown(KeyCode::ALT);
    mods.ctrl = IsKeyDown(KeyCode::CONTROL);
    mods.shift = IsKeyDown(KeyCode::SHIFT);
    return std::move(mods);
}

units::pixel Input::mouse_x() const
{
    return mouse_x_;
}

units::pixel Input::mouse_y() const
{
    return mouse_y_;
}

units::pixel Input::mouse_delta_x() const
{
    return delta_mouse_x_;
}

units::pixel Input::mouse_delta_y() const
{
    return delta_mouse_y_;
}

int Input::mouse_scroll_delta() const
{
    return mouse_scroll_delta_old_;
}

const std::vector<Input::Event>& Input::event_queue() const
{
    return event_queue_;
}

bool Input::Frame()
{
    delta_mouse_x_ = mouse_x_ - old_mouse_x_;
    delta_mouse_y_ = mouse_y_ - old_mouse_y_;
    old_mouse_x_ = mouse_x_;
    old_mouse_y_ = mouse_y_;

    if (delta_mouse_x_ != 0)
    {
        event_queue_buffer_.push_back(Event(Event::MOUSE_MOVE_X, delta_mouse_x_));
    }
    if (delta_mouse_y_ != 0)
    {
        event_queue_buffer_.push_back(Event(Event::MOUSE_MOVE_Y, delta_mouse_y_));
    }

    mouse_scroll_delta_old_ = mouse_scroll_delta_;
    mouse_scroll_delta_ = 0;

    // Since event polling is done before frame is called
    event_queue_ = event_queue_buffer_;
    event_queue_buffer_.clear();

    return true;
}

void Input::Modifiers::Update(Event e)
{
    if (e.type == Event::KEY_DOWN || e.type == Event::KEY_UP)
    {
        if (e.value == KeyCode::ALT)
        {
            alt = (e.type == Event::KEY_DOWN);
        }
        else if (e.value == KeyCode::CONTROL)
        {
            ctrl = (e.type == Event::KEY_DOWN);
        }
        else if (e.value == KeyCode::SHIFT)
        {
            shift = (e.type == Event::KEY_DOWN);
        }
    }
}
} // namespace blons
