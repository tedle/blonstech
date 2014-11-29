#include "graphics/gui/textbox.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
Textbox::Textbox(Box pos, Manager* parent_manager, Window* parent_window)
{
    pos_ = pos;
    gui_ = parent_manager;
    parent_ = parent_window;

    active_ = false;
    text_ = "";
    cursor_ = text_.end();
    // Empty lambda is easier than worrying about nullptrs
    callback_ = [](){};

    // For vertically centering the text
    const auto& font = gui_->skin()->font(FontType::LABEL);
    std::size_t letter_height = font->letter_height();
    Vector2 text_pos;
    text_pos.x = pos.x + gui_->skin()->layout()->textbox.normal.left.w * 2;
    text_pos.y = pos.y + floor((pos.h + letter_height) / 2);
    text_label_ = std::unique_ptr<Label>(new Label(text_pos, text_, gui_, parent_));
}

void Textbox::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();
    auto sprite = gui_->skin()->sprite();
    auto batch = control_batch(context);
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;
    const Skin::Layout::TextboxSetLayout::TextboxLayout* t;
    if (active_)
    {
        t = &layout->textbox.active;
    }
    else
    {
        t = &layout->textbox.normal;
    }

    // Render time
    {
        // Top left corner
        sprite->set_pos(x,
                        y,
                        t->top_left.w,
                        t->top_left.h);
        sprite->set_subtexture(t->top_left);
        batch->Append(*sprite->mesh());

        // Top edge
        sprite->set_pos(x + t->top_left.w,
                        y,
                        pos_.w - (t->top_left.w + t->top_right.w),
                        t->top.h);
        sprite->set_subtexture(t->top);
        batch->Append(*sprite->mesh());

        // Top right corner
        sprite->set_pos(x + pos_.w - t->top_right.w,
                        y,
                        t->top_right.w,
                        t->top_right.h);
        sprite->set_subtexture(t->top_right);
        batch->Append(*sprite->mesh());

        // Left edge
        sprite->set_pos(x,
                        y + t->top_left.h,
                        t->left.w,
                        pos_.h - (t->top_left.h + t->bottom_right.h));
        sprite->set_subtexture(t->left);
        batch->Append(*sprite->mesh());

        // Body
        sprite->set_pos(x + t->left.w,
                        y + t->top.h,
                        pos_.w - (t->left.w + t->right.w),
                        pos_.h - (t->top.h + t->bottom.h));
        sprite->set_subtexture(t->body);
        batch->Append(*sprite->mesh());

        // Right edge
        sprite->set_pos(x + pos_.w - t->right.w,
                        y + t->top_right.h,
                        t->right.w,
                        pos_.h - (t->top_right.h + t->bottom_right.h));
        sprite->set_subtexture(t->right);
        batch->Append(*sprite->mesh());

        // Bottom left corner
        sprite->set_pos(x,
                        y + pos_.h - t->bottom_left.h,
                        t->bottom_left.w,
                        t->bottom_left.h);
        sprite->set_subtexture(t->bottom_left);
        batch->Append(*sprite->mesh());

        // Bottom edge
        sprite->set_pos(x + t->bottom_left.w,
                        y + pos_.h - t->bottom.h,
                        pos_.w - (t->bottom_left.w + t->bottom_right.w),
                        t->bottom.h);
        sprite->set_subtexture(t->bottom);
        batch->Append(*sprite->mesh());

        // Bottom right corner
        sprite->set_pos(x + pos_.w - t->bottom_right.w,
                        y + pos_.h - t->bottom_right.h,
                        t->bottom_right.w,
                        t->bottom_right.h);
        sprite->set_subtexture(t->bottom_right);
        batch->Append(*sprite->mesh());

        if (active_ && cursor_blink_.ms() % 1000 < 500)
        {
            // Text cursor!
            const auto& font = gui_->skin()->font(FontType::LABEL);
            auto cursor_width = 1.0f;
            auto cursor_height = font->letter_height() + 6.0f;
            auto x_offset = font->string_width(std::string(text_.begin(), cursor_), false) + layout->textbox.normal.left.w * 2;
            auto y_offset = floor((pos_.h - cursor_height) / 2);
            sprite->set_pos(x + x_offset,
                y + y_offset,
                cursor_width,
                cursor_height);
            sprite->set_subtexture(layout->textbox.cursor);
            batch->Append(*sprite->mesh());
        }
    }

    RegisterBatches();

    // Button text yall
    text_label_->Render(context);
}

bool Textbox::Update(const Input& input)
{
    bool input_handled = false;

    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    int mx = input.mouse_x();
    int my = input.mouse_y();

    bool shift = input.IsKeyDown(Input::SHIFT);
    bool ctrl = input.IsKeyDown(Input::CONTROL);

    for (const auto& e : input.event_queue())
    {
        if (e.type == Input::Event::MOUSE_DOWN)
        {
            // Clicked inside textbox
            if (mx >= x && mx < x + pos_.w &&
                my >= y && my < y + pos_.h)
            {
                active_ = true;
                cursor_ = text_.end();
                input_handled = true;
                cursor_blink_.start();
            }
            else
            {
                active_ = false;
                cursor_blink_.stop();
            }
        }
        if (active_)
        {
            auto key = static_cast<Input::KeyCode>(e.value);

            if (e.type == Input::Event::KEY_DOWN)
            {
                if (input.IsPrintable(key) && !ctrl)
                {
                    cursor_ = text_.insert(cursor_, input.ToAscii(key, shift)) + 1;
                }
                else if (key == Input::BACKSPACE)
                {
                    if (cursor_ > text_.begin())
                    {
                        cursor_ = text_.erase(cursor_ - 1);
                    }
                }
                else if (key == Input::DEL)
                {
                    if (cursor_ < text_.end())
                    {
                        cursor_ = text_.erase(cursor_);
                    }
                }
                else if (key == Input::LEFT)
                {
                    if (cursor_ > text_.begin())
                    {
                        cursor_--;
                    }
                }
                else if (key == Input::RIGHT)
                {
                    if (cursor_ < text_.end())
                    {
                        cursor_++;
                    }
                }
                else if (key == Input::SHIFT)
                {
                    shift = true;
                }
                else if (key == Input::CONTROL)
                {
                    ctrl = true;
                }
                else if (key == Input::RETURN)
                {
                    callback_();
                }
                text_label_->set_text(text_);
            }
            else if (e.type == Input::Event::KEY_UP)
            {
                if (key == Input::SHIFT)
                {
                    shift = false;
                }
                else if (key == Input::CONTROL)
                {
                    ctrl = false;
                }
            }
        }
    }
    input_handled |= active_;

    return input_handled;
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
    cursor_ = text_.end();
}
} // namespace GUI
} // namespace blons