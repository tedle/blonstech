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

    // For vertically centering the text
    const auto& font = gui_->skin()->font(FontType::LABEL);
    int letter_height = font->letter_height();
    Vector2 text_pos;
    text_pos.x = pos.x + gui_->skin()->layout()->textbox.normal.left.w * 2;
    text_pos.y = pos.y + floor((pos.h + letter_height) / 2);
    text_ = std::unique_ptr<Label>(new Label(text_pos, "Hello!", gui_, parent_));
}

void Textbox::Render(RenderContext& context)
{
    const Skin::Layout::TextboxSetLayout::TextboxLayout* t;
    if (active_)
    {
        t = &gui_->skin()->layout()->textbox.active;
    }
    else
    {
        t = &gui_->skin()->layout()->textbox.normal;
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
    }

    RegisterBatches();

    // Button text yall
    text_->Render(context);
}

bool Textbox::Update(const Input& input)
{
    bool input_handled = false;

    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    int mx = input.mouse_x();
    int my = input.mouse_y();

    for (const auto& e : input.event_queue())
    {
        if (e.type == Input::Event::MOUSE_DOWN)
        {
            // Clicked inside textbox
            if (mx >= x && mx < x + pos_.w &&
                my >= y && my < y + pos_.h)
            {
                active_ = true;
                input_handled = true;
            }
            else
            {
                active_ = false;
            }
        }
    }

    return input_handled;
}
} // namespace GUI
} // namespace blons