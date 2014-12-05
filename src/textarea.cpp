#include "graphics/gui/textarea.h"

// Includes
#include <algorithm>
// Local Includes
#include "math/animation.h"
#include "graphics/gui/gui.h"
#include "graphics/gui/label.h"

namespace blons
{
namespace GUI
{
Textarea::Textarea(Box pos, FontStyle style, Manager* parent_manager, Window* parent_window)
{
    pos_ = pos;
    font_style_ = style;
    gui_ = parent_manager;
    parent_ = parent_window;

    newest_top_ = false;
    padding_ = units::subpixel_to_pixel(gui_->skin()->layout()->textarea.left.w * 2);

    scroll_offset_ = 0;
    scroll_start_ = 0;
    scroll_destination_ = 0;

    Animation::Callback callback = [this](float y)
    {
        scroll_offset_ = scroll_start_ + units::subpixel_to_pixel((scroll_destination_ - scroll_start_) * y);
    };
    scroll_animation_ = std::unique_ptr<Animation>(new Animation(250, callback, Animation::CUBIC_OUT));

    scroll_timer_.start();
}

void Textarea::Render(RenderContext& context)
{
    auto layout = gui_->skin()->layout();
    auto sprite = gui_->skin()->sprite();
    auto batch = control_batch(context);
    auto parent_pos = parent_->pos();
    auto x = pos_.x + parent_pos.x;
    auto y = pos_.y + parent_pos.y;

    const auto& t = layout->textarea;
    // Render time
    {
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
    }

    RegisterBatches();

    // Render the text, only grab lines that are at visible
    const auto& font = gui_->skin()->font(font_style_);
    auto line_offset = scroll_offset_ / font->line_height();
    // You only need a height of 2 pixels to be able to see 2 lines, so thats minimum
    std::size_t renderable_lines = std::min(lines_.size() - line_offset,
                                            static_cast<std::size_t>((pos_.h / font->line_height()) + 2));

    for (auto i = line_offset; i < renderable_lines + line_offset; i++)
    {
        auto& label = lines_[lines_.size() - i - 1];
        label->set_crop(Box(0.0f, y + padding_ / 2, 0.0f, pos_.h - padding_), 5);

        if (newest_top_)
        {
            label->set_pos(pos_.x + padding_, pos_.y + padding_ + i * font->line_height());
        }
        else
        {
            label->set_pos(pos_.x + padding_, pos_.y + pos_.h - padding_ - i * font->line_height() + scroll_offset_);
        }

        label->Render(context);
    }
}

bool Textarea::Update(const Input& input)
{
    bool input_handled = false;

    scroll_animation_->Update();
    auto us = scroll_timer_.us();
    for (const auto& e : input.event_queue())
    {
        // TODO: Make a more sensible if block
        if (gui_->active_window() == parent_)
        {
            if (e.type == Input::Event::KEY_DOWN)
            {
                const auto& font = gui_->skin()->font(font_style_);
                const auto ph = units::subpixel_to_pixel(pos_.h);
                auto page_offset = std::max(font->line_height(), ph - (ph % font->line_height()));
                if (e.value == Input::KeyCode::PG_UP)
                {
                    MoveScrollOffset(page_offset);
                    input_handled = true;
                }
                else if (e.value == Input::KeyCode::PG_DOWN)
                {
                    MoveScrollOffset(-page_offset);
                    input_handled = true;
                }

            }
        }
        // Handle these even when unfocused, so long as mouse pointer is inside textarea
        if (e.type == Input::Event::MOUSE_SCROLL)
        {
            auto parent_pos = parent_->pos();
            auto x = pos_.x + parent_pos.x;
            auto y = pos_.y + parent_pos.y;

            units::pixel mx = input.mouse_x();
            units::pixel my = input.mouse_y();

            // Mouse inside textarea
            if (mx >= x && mx < x + pos_.w &&
                my >= y && my < y + pos_.h)
            {
                const auto& font = gui_->skin()->font(font_style_);
                MoveScrollOffset(font->line_height() * e.value * 3);
            }
        }
    }
    return input_handled;
}

void Textarea::GenLabel(std::string text)
{
    const auto& font = gui_->skin()->font(font_style_);
    auto lines = font->string_wrap(text, units::subpixel_to_pixel(pos_.w) - padding_ * 2);
    if (newest_top_)
    {
        for (auto line = lines.rbegin(); line != lines.rend(); line++)
        {
            auto label = std::unique_ptr<Label>(new Label(Vector2(0, 0), *line, font_style_, gui_, parent_));
            lines_.push_back(std::move(label));
        }
    }
    else
    {
        for (const auto& line : lines)
        {
            auto label = std::unique_ptr<Label>(new Label(Vector2(0, 0), line, font_style_, gui_, parent_));
            lines_.push_back(std::move(label));
        }
    }
}

void Textarea::AddLine(std::string text)
{
    history_.push_back(text);
    GenLabel(text);
}

void Textarea::Clear()
{
    history_.clear();
    lines_.clear();

    scroll_start_ = 0;
    scroll_offset_ = 0;
    scroll_destination_ = 0;
}

void Textarea::MoveScrollOffset(units::pixel delta)
{
    const auto& font = gui_->skin()->font(font_style_);
    units::pixel max_offset = font->line_height() * static_cast<units::pixel>(lines_.size())
                            - units::subpixel_to_pixel(pos_.h)
                            + font->letter_height()
                            - padding_ / 2;

    scroll_start_ = scroll_offset_;
    scroll_destination_ = std::max(0, std::min(scroll_destination_ + delta, max_offset));
    scroll_animation_->Reset();
}
} // namespace GUI
} // namespace blons