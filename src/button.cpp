#include "graphics/gui/button.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
Button::Button(int x, int y, int width, int height, const char* label, Manager* parent_manager, Window* parent_window)
{
    pos_ = Box(static_cast<float>(x),
               static_cast<float>(y),
               static_cast<float>(width),
               static_cast<float>(height));
    gui_ = parent_manager;
    parent_ = parent_window;
    hover_ = false;
    active_ = false;
    // Empty lambda is easier than worrying about nullptrs
    callback_ = [](){};

    if (strlen(label) > 0)
    {
        const auto& font = gui_->skin()->font(FontType::LABEL);
        int caption_width = font->string_width(label);
        int letter_height = font->letter_height();
        // For centering the button caption
        int caption_x_offset = (width - caption_width) / 2 - font->cursor_offset(label[0]);
        int caption_y_offset = (height + letter_height) / 2;
        label_ = std::unique_ptr<Label>(new Label(x + caption_x_offset, y + caption_y_offset,
                                                  label, parent_manager, parent_window));
    }
}

void Button::Render(RenderContext& context)
{
    const Skin::Layout::ButtonSetLayout::ButtonLayout* b;
    if (active_ && hover_)
    {
        b = &gui_->skin()->layout()->button.active;
    }
    else if (hover_)
    {
        b = &gui_->skin()->layout()->button.hover;
    }
    else
    {
        b = &gui_->skin()->layout()->button.normal;
    }

    auto sprite = gui_->skin()->sprite();
    auto batch = control_batch(context);
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    // Render time
    {
        // Top left corner
        sprite->set_pos(x,
                        y,
                        b->top_left.w,
                        b->top_left.h);
        sprite->set_subtexture(b->top_left);
        batch->Append(*sprite->mesh());

        // Top edge
        sprite->set_pos(x + b->top_left.w,
                        y,
                        pos_.w - (b->top_left.w + b->top_right.w),
                        b->top.h);
        sprite->set_subtexture(b->top);
        batch->Append(*sprite->mesh());

        // Top right corner
        sprite->set_pos(x + pos_.w - b->top_right.w,
                        y,
                        b->top_right.w,
                        b->top_right.h);
        sprite->set_subtexture(b->top_right);
        batch->Append(*sprite->mesh());

        // Left edge
        sprite->set_pos(x,
                        y + b->top_left.h,
                        b->left.w,
                        pos_.h - (b->top_left.h + b->bottom_right.h));
        sprite->set_subtexture(b->left);
        batch->Append(*sprite->mesh());

        // Body
        sprite->set_pos(x + b->left.w,
                        y + b->top.h,
                        pos_.w - (b->left.w + b->right.w),
                        pos_.h - (b->top.h + b->bottom.h));
        sprite->set_subtexture(b->body);
        batch->Append(*sprite->mesh());

        // Right edge
        sprite->set_pos(x + pos_.w - b->right.w,
                        y + b->top_right.h,
                        b->right.w,
                        pos_.h - (b->top_right.h + b->bottom_right.h));
        sprite->set_subtexture(b->right);
        batch->Append(*sprite->mesh());

        // Bottom left corner
        sprite->set_pos(x,
                        y + pos_.h - b->bottom_left.h,
                        b->bottom_left.w,
                        b->bottom_left.h);
        sprite->set_subtexture(b->bottom_left);
        batch->Append(*sprite->mesh());

        // Bottom edge
        sprite->set_pos(x + b->bottom_left.w,
                        y + pos_.h - b->bottom.h,
                        pos_.w - (b->bottom_left.w + b->bottom_right.w),
                        b->bottom.h);
        sprite->set_subtexture(b->bottom);
        batch->Append(*sprite->mesh());

        // Bottom right corner
        sprite->set_pos(x + pos_.w - b->bottom_right.w,
                        y + pos_.h - b->bottom_right.h,
                        b->bottom_right.w,
                        b->bottom_right.h);
        sprite->set_subtexture(b->bottom_right);
        batch->Append(*sprite->mesh());
    }

    RegisterBatches();

    // Button text yall
    label_->Render(context);
}

bool Button::Update(const Input& input)
{
    bool input_handled = false;

    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    int mx = input.mouse_x();
    int my = input.mouse_y();

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
                if (active_ && hover_)
                {
                    callback_();
                }
                active_ = false;
                input_handled = true;
            }
        }
    }
    // Swallow input while button is held
    input_handled |= active_;

    return input_handled;
}
} // namespace GUI
} // namespace blons

