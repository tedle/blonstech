#include <graphics/gui/textbox.h>

// Local Includes
#include <graphics/gui/gui.h>

namespace blons
{
namespace gui
{
Textbox::Textbox(Box pos, FontStyle style, Manager* parent_manager, Window* parent_window)
{
    pos_ = pos;
    gui_ = parent_manager;
    parent_ = parent_window;

    active_ = false;
    text_ = "";
    font_style_ = style;
    cursor_ = text_.end();
    cursor_blink_.start();
    // Empty lambda is easier than worrying about nullptrs
    callback_ = [](){};

    padding_ = units::subpixel_to_pixel(gui_->skin()->layout()->textbox.normal.left.w * 2);
    // For vertically centering the text
    const auto& font = gui_->skin()->font(font_style_);
    std::size_t letter_height = font->letter_height();
    Vector2 text_pos;
    text_pos.x = pos.x + padding_;
    text_pos.y = pos.y + floor((pos.h + letter_height) / 2);
    text_label_ = std::unique_ptr<Label>(new Label(text_pos, text_, style, gui_, parent_));
    text_label_->set_colour_parsing(false);
}

void Textbox::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    const Skin::Layout::Textbox* textbox_layout;
    if (active_)
    {
        textbox_layout = &layout->textbox.active;
    }
    else
    {
        textbox_layout = &layout->textbox.normal;
    }

    RenderBody(*textbox_layout, context);
    if (active_)
    {
        RenderCursor(layout->textbox.cursor, context);
    }

    RegisterBatches();

    RenderText(context);
}

void Textbox::RenderBody(const Skin::Layout::Textbox& t, RenderContext& context)
{
    auto sprite = gui_->skin()->sprite();
    auto batch = control_batch(context);
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    // Top left corner
    sprite->set_pos(x,
                    y,
                    t.top_left.w,
                    t.top_left.h);
    sprite->set_subtexture(t.top_left);
    batch->Append(*sprite->mesh());

    // Top edge
    sprite->set_pos(x + t.top_left.w,
                    y,
                    pos_.w - (t.top_left.w + t.top_right.w),
                    t.top.h);
    sprite->set_subtexture(t.top);
    batch->Append(*sprite->mesh());

    // Top right corner
    sprite->set_pos(x + pos_.w - t.top_right.w,
                    y,
                    t.top_right.w,
                    t.top_right.h);
    sprite->set_subtexture(t.top_right);
    batch->Append(*sprite->mesh());

    // Left edge
    sprite->set_pos(x,
                    y + t.top_left.h,
                    t.left.w,
                    pos_.h - (t.top_left.h + t.bottom_right.h));
    sprite->set_subtexture(t.left);
    batch->Append(*sprite->mesh());

    // Body
    sprite->set_pos(x + t.left.w,
                    y + t.top.h,
                    pos_.w - (t.left.w + t.right.w),
                    pos_.h - (t.top.h + t.bottom.h));
    sprite->set_subtexture(t.body);
    batch->Append(*sprite->mesh());

    // Right edge
    sprite->set_pos(x + pos_.w - t.right.w,
                    y + t.top_right.h,
                    t.right.w,
                    pos_.h - (t.top_right.h + t.bottom_right.h));
    sprite->set_subtexture(t.right);
    batch->Append(*sprite->mesh());

    // Bottom left corner
    sprite->set_pos(x,
                    y + pos_.h - t.bottom_left.h,
                    t.bottom_left.w,
                    t.bottom_left.h);
    sprite->set_subtexture(t.bottom_left);
    batch->Append(*sprite->mesh());

    // Bottom edge
    sprite->set_pos(x + t.bottom_left.w,
                    y + pos_.h - t.bottom.h,
                    pos_.w - (t.bottom_left.w + t.bottom_right.w),
                    t.bottom.h);
    sprite->set_subtexture(t.bottom);
    batch->Append(*sprite->mesh());

    // Bottom right corner
    sprite->set_pos(x + pos_.w - t.bottom_right.w,
                    y + pos_.h - t.bottom_right.h,
                    t.bottom_right.w,
                    t.bottom_right.h);
    sprite->set_subtexture(t.bottom_right);
    batch->Append(*sprite->mesh());

    // Label base colour
    text_label_->set_text_colour(t.colour);
}

void Textbox::RenderCursor(const Box& cursor, RenderContext& context)
{
    auto sprite = gui_->skin()->sprite();
    auto batch = control_batch(context);
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    if (cursor_blink_.ms() % 1000 < 500)
    {
        auto cursor_width = 1.0f;
        auto cursor_height = gui_->skin()->font(font_style_)->letter_height() + 6.0f;
        auto x_offset = CursorOffset();
        auto y_offset = floor((pos_.h - cursor_height) / 2);
        sprite->set_pos(x + x_offset,
                        y + y_offset,
                        cursor_width,
                        cursor_height);
        sprite->set_subtexture(cursor);
        batch->Append(*sprite->mesh());
    }
}

void Textbox::RenderText(RenderContext& context)
{
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;

    text_label_->set_crop(Box(x + padding_ / 2, 0.0f, pos_.w - padding_, 0.0f), padding_ / 2);
    text_label_->Render(context);
}

bool Textbox::Update(const Input& input)
{
    auto mods = input.modifiers();
    auto events = GetEventsWithRepeats(input);

    for (const auto& e : events)
    {
        if (e.type == Input::Event::MOUSE_DOWN)
        {
            OnMouseDown(input);
        }
        if (active_)
        {
            auto key = static_cast<Input::KeyCode>(e.value);
            mods.Update(e);

            if (e.type == Input::Event::KEY_DOWN)
            {
                OnKeyDown(input, key, mods);
            }
            else if (e.type == Input::Event::KEY_UP)
            {
                OnKeyUp(input, key, mods);
            }
        }
    }

    // Consume input while active
    return active_;
}

void Textbox::set_callback(std::function<void()> callback)
{
    callback_ = callback;
}

std::string Textbox::text() const
{
    return text_;
}

void Textbox::set_text(std::string text)
{
    text_ = text;
    text_label_->set_text(text_);
    SetCursorPos(text_.end());
}

std::vector<Input::Event> Textbox::GetEventsWithRepeats(const Input& input)
{
    // Make a mutable copy so we can inject repeated key presses
    auto events(input.event_queue());
    if (key_repeat_.timer.ms() > 500)
    {
        auto code = key_repeat_.code;
        if (input.IsPrintable(code) ||
            code == Input::BACKSPACE ||
            code == Input::DEL ||
            code == Input::LEFT ||
            code == Input::RIGHT)
        {
            Input::Event repeat(Input::Event::KEY_DOWN, code);
            events.push_back(repeat);
            // How long before next repeated key press
            key_repeat_.timer.rewind(40);
        }
    }
    return std::move(events);
}

void Textbox::OnMouseDown(const Input& input)
{
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    units::pixel mx = input.mouse_x();
    units::pixel my = input.mouse_y();

    // Clicked inside textbox
    if (mx >= x && mx < x + pos_.w &&
        my >= y && my < y + pos_.h)
    {
        active_ = true;
        cursor_blink_.start();
        cursor_ = text_.end();
    }
    else
    {
        active_ = false;
        cursor_blink_.stop();
        key_repeat_.timer.stop();
        key_repeat_.code = Input::BAD;
    }
}

void Textbox::OnKeyDown(const Input& input, const Input::KeyCode key, Input::Modifiers mods)
{
    // Easier to follow when it's moving around
    cursor_blink_.start();

    if (key != key_repeat_.code)
    {
        key_repeat_.timer.start();
        key_repeat_.code = key;
    }

    if (input.IsPrintable(key) && !mods.ctrl && !mods.alt)
    {
        SetCursorPos(text_.insert(cursor_, input.ToAscii(key, mods.shift)) + 1);
    }
    else if (key == Input::BACKSPACE)
    {
        if (cursor_ > text_.begin())
        {
            SetCursorPos(text_.erase(cursor_ - 1));
        }
    }
    else if (key == Input::DEL)
    {
        if (cursor_ < text_.end())
        {
            SetCursorPos(text_.erase(cursor_));
        }
    }
    else if (key == Input::LEFT)
    {
        if (cursor_ > text_.begin())
        {
            SetCursorPos(cursor_ - 1);
        }
    }
    else if (key == Input::RIGHT)
    {
        if (cursor_ < text_.end())
        {
            SetCursorPos(cursor_ + 1);
        }
    }
    else if (key == Input::RETURN)
    {
        callback_();
    }
    text_label_->set_text(text_);
}

void Textbox::OnKeyUp(const Input& input, const Input::KeyCode key, Input::Modifiers mods)
{
    if (key == key_repeat_.code)
    {
        key_repeat_.timer.stop();
        key_repeat_.code = Input::BAD;
    }
    // Restart timer if we let up a key that's not being repeated
    else
    {
        key_repeat_.timer.start();
    }
}

void Textbox::SetCursorPos(std::string::iterator cursor)
{
    cursor_ = cursor;
    const auto label_pos = text_label_->pos();
    const auto cursor_offset = CursorOffset();
    // Cursor out of bounds right side
    if (cursor_offset >= pos_.w - padding_)
    {
        auto label_diff = cursor_offset - (pos_.w - padding_) + 1;
        text_label_->set_pos(label_pos.x - label_diff, label_pos.y);
    }
    // Cursor out of bounds left side
    else if (cursor_offset < padding_)
    {
        auto label_diff = cursor_offset - padding_;
        text_label_->set_pos(label_pos.x - label_diff, label_pos.y);
    }
}

units::subpixel Textbox::CursorOffset()
{
    const auto font = gui_->skin()->font(font_style_);
    const auto layout = gui_->skin()->layout();
    const auto label_offset = text_label_->pos().x - (pos_.x + padding_);
    const auto cursor_offset = font->string_width(std::string(text_.begin(), cursor_), false) + label_offset + padding_;
    return cursor_offset;
}
} // namespace gui
} // namespace blons