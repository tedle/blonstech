#include "graphics/gui/window.h"

// Local Includes
#include "graphics/gui/gui.h"

namespace blons
{
namespace GUI
{
Window::Window(int x, int y, int width, int height, WindowType type, Manager* parent_manager)
{
    pos_ = Box(static_cast<float>(x),
               static_cast<float>(y),
               static_cast<float>(width),
               static_cast<float>(height));
    type_ = type;
    gui_ = parent_manager;
}

void Window::Render(RenderContext& context)
{
    // Render self
    auto skin = gui_->skin();
    auto layout = skin->layout();
    auto sprite = skin->sprite();
    // Let's get ugly
    // Title bar
    {
        auto& t = layout->window.title;
        // Left
        sprite->set_pos(pos_.x,
                        pos_.y,
                        t.left.w,
                        t.left.h);
        sprite->set_subtexture(t.left);
        gui_->control_batch()->Append(*sprite->mesh());
        // Center
        sprite->set_pos(pos_.x + t.left.w,
                        pos_.y,
                        pos_.w - (t.left.w + t.right.w),
                        t.left.h);
        sprite->set_subtexture(t.center);
        gui_->control_batch()->Append(*sprite->mesh());
        // Right
        sprite->set_pos(pos_.x + pos_.w - t.right.w,
                        pos_.y,
                        t.right.w,
                        t.right.h);
        sprite->set_subtexture(t.right);
        gui_->control_batch()->Append(*sprite->mesh());
    }
    // Body
    {
        // Add offset if window has a title bar
        float t_off = 0.0;
        if (type_ == WindowType::DRAGGABLE)
        {
            t_off = layout->window.title.center.h;
        }

        auto& b = layout->window;

        // Top left corner
        sprite->set_pos(pos_.x,
                        pos_.y + t_off,
                        b.top_left.w,
                        b.top_left.h);
        sprite->set_subtexture(b.top_left);
        gui_->control_batch()->Append(*sprite->mesh());

        // Top edge
        sprite->set_pos(pos_.x + b.top_left.w,
                        pos_.y + t_off,
                        pos_.w - (b.top_left.w + b.top_right.w),
                        b.top.h);
        sprite->set_subtexture(b.top);
        gui_->control_batch()->Append(*sprite->mesh());

        // Top right corner
        sprite->set_pos(pos_.x + pos_.w - b.top_right.w,
                        pos_.y + t_off,
                        b.top_right.w,
                        b.top_right.h);
        sprite->set_subtexture(b.top_right);
        gui_->control_batch()->Append(*sprite->mesh());

        // Left edge
        sprite->set_pos(pos_.x,
                        pos_.y + b.top_left.h + t_off,
                        b.left.w,
                        pos_.h - (b.top_left.h + b.bottom_right.h) - t_off);
        sprite->set_subtexture(b.left);
        gui_->control_batch()->Append(*sprite->mesh());

        // Body
        sprite->set_pos(pos_.x + b.left.w,
                        pos_.y + b.top.h + t_off,
                        pos_.w - (b.left.w + b.right.w),
                        pos_.h - (b.top.h + b.bottom.h) - t_off);
        sprite->set_subtexture(b.body);
        gui_->control_batch()->Append(*sprite->mesh());

        // Right edge
        sprite->set_pos(pos_.x + pos_.w - b.right.w,
                        pos_.y + b.top_right.h + t_off,
                        b.right.w,
                        pos_.h - (b.top_right.h + b.bottom_right.h) - t_off);
        sprite->set_subtexture(b.right);
        gui_->control_batch()->Append(*sprite->mesh());

        // Bottom left corner
        sprite->set_pos(pos_.x,
                        pos_.y + pos_.w - b.bottom_left.h,
                        b.bottom_left.w,
                        b.bottom_left.h);
        sprite->set_subtexture(b.bottom_left);
        gui_->control_batch()->Append(*sprite->mesh());

        // Bottom edge
        sprite->set_pos(pos_.x + b.bottom_left.w,
                        pos_.y + pos_.w - b.bottom.h,
                        pos_.w - (b.bottom_left.w + b.bottom_right.w),
                        b.bottom.h);
        sprite->set_subtexture(b.bottom);
        gui_->control_batch()->Append(*sprite->mesh());

        // Bottom right corner
        sprite->set_pos(pos_.x + pos_.w - b.bottom_right.w,
                        pos_.y + pos_.w - b.bottom_right.h,
                        b.bottom_right.w,
                        b.bottom_right.h);
        sprite->set_subtexture(b.bottom_right);
        gui_->control_batch()->Append(*sprite->mesh());
    }

    // Render controls
    for (const auto& control : controls_)
    {
        control->Render(context);
    }
}

bool Window::Update(const Input& input)
{
    for (const auto& e : input.event_queue())
    {
        if (e.type == Input::Event::MOUSE_DOWN)
        {
            int mx = input.mouse_x();
            int my = input.mouse_y();
            if (mx >= pos_.x && mx < pos_.x + pos_.w &&
                my >= pos_.y && my < pos_.y + pos_.h)
            {
                if (type_ != INVISIBLE)
                {
                    gui_->set_active_window(this);
                }
                return true;
            }
        }
    }
    return false;
}

Label* Window::CreateLabel(int x, int y, const char* text)
{
    std::unique_ptr<Label> label(new Label(x, y, text, gui_));
    controls_.push_back(std::move(label));
    return static_cast<Label*>(controls_.back().get());
}
} // namespace GUI
} // namespace blons
