#include <blons/graphics/gui/button.h>

// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
Button::Button(Box pos, std::string label, Manager* parent_manager, Window* parent_window)
    : Control(pos, parent_manager, parent_window)
{
    hover_ = false;
    active_ = false;
    // Empty lambda is easier than worrying about nullptrs
    callback_ = [](){};

    if (label.length() > 0)
    {
        const auto& font = gui_->skin()->font(FontStyle::LABEL);
        units::pixel caption_width = font->string_width(label);
        units::pixel letter_height = font->letter_height();
        // Vertically center the button caption
        Vector2 caption_pos;
        caption_pos.x = pos.x + floor((pos.w - caption_width) / 2 - font->cursor_offset(label[0]));
        caption_pos.y = pos.y + floor((pos.h + letter_height) / 2);
        label_.reset(new Label(caption_pos, label, parent_manager, parent_window));
    }
}

void Button::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();

    const Skin::Layout::Button* b;
    if (active_ && hover_)
    {
        b = &layout->button.active;
    }
    else if (hover_)
    {
        b = &layout->button.hover;
    }
    else
    {
        b = &layout->button.normal;
    }

    RenderBody(*b, context);
    RegisterBatches();

    // Button text yall
    label_->Render(context);
}

void Button::RenderBody(const Skin::Layout::Button& b, RenderContext& context)
{
    auto sprite = gui_->skin()->sprite();
    auto batch = control_batch(context);
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    // Top left corner
    sprite->set_pos(x,
                    y,
                    b.top_left.w,
                    b.top_left.h);
    sprite->set_subtexture(b.top_left);
    batch->Append(sprite->mesh(), context);

    // Top edge
    sprite->set_pos(x + b.top_left.w,
                    y,
                    pos_.w - (b.top_left.w + b.top_right.w),
                    b.top.h);
    sprite->set_subtexture(b.top);
    batch->Append(sprite->mesh(), context);

    // Top right corner
    sprite->set_pos(x + pos_.w - b.top_right.w,
                    y,
                    b.top_right.w,
                    b.top_right.h);
    sprite->set_subtexture(b.top_right);
    batch->Append(sprite->mesh(), context);

    // Left edge
    sprite->set_pos(x,
                    y + b.top_left.h,
                    b.left.w,
                    pos_.h - (b.top_left.h + b.bottom_right.h));
    sprite->set_subtexture(b.left);
    batch->Append(sprite->mesh(), context);

    // Body
    sprite->set_pos(x + b.left.w,
                    y + b.top.h,
                    pos_.w - (b.left.w + b.right.w),
                    pos_.h - (b.top.h + b.bottom.h));
    sprite->set_subtexture(b.body);
    batch->Append(sprite->mesh(), context);

    // Right edge
    sprite->set_pos(x + pos_.w - b.right.w,
                    y + b.top_right.h,
                    b.right.w,
                    pos_.h - (b.top_right.h + b.bottom_right.h));
    sprite->set_subtexture(b.right);
    batch->Append(sprite->mesh(), context);

    // Bottom left corner
    sprite->set_pos(x,
                    y + pos_.h - b.bottom_left.h,
                    b.bottom_left.w,
                    b.bottom_left.h);
    sprite->set_subtexture(b.bottom_left);
    batch->Append(sprite->mesh(), context);

    // Bottom edge
    sprite->set_pos(x + b.bottom_left.w,
                    y + pos_.h - b.bottom.h,
                    pos_.w - (b.bottom_left.w + b.bottom_right.w),
                    b.bottom.h);
    sprite->set_subtexture(b.bottom);
    batch->Append(sprite->mesh(), context);

    // Bottom right corner
    sprite->set_pos(x + pos_.w - b.bottom_right.w,
                    y + pos_.h - b.bottom_right.h,
                    b.bottom_right.w,
                    b.bottom_right.h);
    sprite->set_subtexture(b.bottom_right);
    batch->Append(sprite->mesh(), context);

    // Label base colour
    label_->set_text_colour(b.colour);
}

bool Button::Update(const Input& input)
{
    bool input_handled = false;

    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    units::pixel mx = input.mouse_x();
    units::pixel my = input.mouse_y();

    // Cursor inside button
    if (mx >= x && mx < x + pos_.w &&
        my >= y && my < y + pos_.h)
    {
        hover_ = true;
    }
    else
    {
        hover_ = false;
    }

    for (const auto& e : input.event_queue())
    {
        // Clicked inside button
        if (mx >= x && mx < x + pos_.w &&
            my >= y && my < y + pos_.h)
        {
            if (e.type == Input::Event::MOUSE_DOWN)
            {
                active_ = true;
                input_handled = true;
            }
            else if (e.type == Input::Event::MOUSE_UP)
            {
                // Only do callback if cursor is inside button's region when mouse is released
                if (active_ && hover_)
                {
                    callback_();
                }
                active_ = false;
                input_handled = true;
            }
        }
        else
        {
            if (e.type == Input::Event::MOUSE_UP)
            {
                active_ = false;
            }
        }
    }
    // Swallow input while button is held
    input_handled |= active_;

    return input_handled;
}

void Button::set_callback(std::function<void()> callback)
{
    callback_ = callback;
}
} // namespace gui
} // namespace blons

