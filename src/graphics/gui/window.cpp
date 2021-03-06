////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <blons/graphics/gui/window.h>

// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
Window::Window(Box pos, std::string caption, Type type, Manager* parent_manager)
    : Control(pos, parent_manager, nullptr), type_(type)
{
    dragging_ = false;
    drag_offset_ = Vector2(0, 0);

    units::subpixel title_bar_height = gui_->skin()->layout()->window.title.center.h;
    units::subpixel letter_height = units::pixel_to_subpixel(gui_->skin()->font(Skin::FontStyle::HEADING)->letter_height());
    units::subpixel caption_offset = (title_bar_height + letter_height) / 2;
    Vector2 caption_pos(gui_->skin()->layout()->window.title.left.w, caption_offset);
    for (auto& c : caption)
    {
        c = toupper(c);
    }
    Vector4 colour = gui_->skin()->layout()->window.title.colour;
    caption_.reset(new Label(caption_pos, ColourString(caption, colour), Skin::FontStyle::HEADING, parent_manager, this));
}

void Window::Render()
{
    if (type_ != INVISIBLE)
    {
        RenderBody();
    }

    if (type_ == DRAGGABLE)
    {
        caption_->Render();
    }

    // Render controls
    for (const auto& control : controls_)
    {
        if (!control->hidden())
        {
            control->Render();
        }
    }
}

void Window::RenderBody()
{
    auto skin = gui_->skin();
    auto layout = skin->layout();
    Box pos, uv;
    // Title bar
    if (type_ == Window::DRAGGABLE)
    {
        auto& t = layout->window.title;
        // Left
        pos.x = pos_.x;
        pos.y = pos_.y;
        pos.w = t.left.w;
        pos.h = t.left.h;
        uv = t.left;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
        // Center
        pos.x = pos_.x + t.left.w;
        pos.y = pos_.y;
        pos.w = pos_.w - (t.left.w + t.right.w);
        pos.h = t.left.h;
        uv = t.center;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
        // Right
        pos.x = pos_.x + pos_.w - t.right.w;
        pos.y = pos_.y;
        pos.w = t.right.w;
        pos.h = t.right.h;
        uv = t.right;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
    }
    // Body
    {
        // Add offset if window has a title bar
        units::subpixel t_off = 0.0;
        if (type_ == Window::DRAGGABLE)
        {
            t_off = layout->window.title.center.h;
        }

        auto& b = layout->window;

        // Top left corner
        pos.x = pos_.x;
        pos.y = pos_.y + t_off;
        pos.w = b.top_left.w;
        pos.h = b.top_left.h;
        uv = b.top_left;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Top edge
        pos.x = pos_.x + b.top_left.w;
        pos.y = pos_.y + t_off;
        pos.w = pos_.w - (b.top_left.w + b.top_right.w);
        pos.h = b.top.h;
        uv = b.top;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Top right corner
        pos.x = pos_.x + pos_.w - b.top_right.w;
        pos.y = pos_.y + t_off;
        pos.w = b.top_right.w;
        pos.h = b.top_right.h;
        uv = b.top_right;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Left edge
        pos.x = pos_.x;
        pos.y = pos_.y + b.top_left.h + t_off;
        pos.w = b.left.w;
        pos.h = pos_.h - (b.top_left.h + b.bottom_right.h) - t_off;
        uv = b.left;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Body
        pos.x = pos_.x + b.left.w;
        pos.y = pos_.y + b.top.h + t_off;
        pos.w = pos_.w - (b.left.w + b.right.w);
        pos.h = pos_.h - (b.top.h + b.bottom.h) - t_off;
        uv = b.body;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Right edge
        pos.x = pos_.x + pos_.w - b.right.w;
        pos.y = pos_.y + b.top_right.h + t_off;
        pos.w = b.right.w;
        pos.h = pos_.h - (b.top_right.h + b.bottom_right.h) - t_off;
        uv = b.right;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Bottom left corner
        pos.x = pos_.x;
        pos.y = pos_.y + pos_.h - b.bottom_left.h;
        pos.w = b.bottom_left.w;
        pos.h = b.bottom_left.h;
        uv = b.bottom_left;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Bottom edge
        pos.x = pos_.x + b.bottom_left.w;
        pos.y = pos_.y + pos_.h - b.bottom.h;
        pos.w = pos_.w - (b.bottom_left.w + b.bottom_right.w);
        pos.h = b.bottom.h;
        uv = b.bottom;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);

        // Bottom right corner
        pos.x = pos_.x + pos_.w - b.bottom_right.w;
        pos.y = pos_.y + pos_.h - b.bottom_right.h;
        pos.w = b.bottom_right.w;
        pos.h = b.bottom_right.h;
        uv = b.bottom_right;
        gui_->SubmitControlBatch(pos, uv, crop_, feather_);
    }
}

bool Window::Update(const Input& input)
{
    bool input_handled = false;

    units::pixel mx = input.mouse_x();
    units::pixel my = input.mouse_y();

    for (const auto& e : input.event_queue())
    {
        if (e.type == Input::Event::MOUSE_DOWN)
        {
            // Clicked inside window
            if (mx >= pos_.x && mx < pos_.x + pos_.w &&
                my >= pos_.y && my < pos_.y + pos_.h)
            {
                if (type_ != INVISIBLE)
                {
                    gui_->set_active_window(this);
                    input_handled = true;
                }
                // Clicked inside title bar
                if (type_ == DRAGGABLE &&
                    my < pos_.y + gui_->skin()->layout()->window.title.center.h)
                {
                    dragging_ = true;
                    drag_offset_.x = mx - pos_.x;
                    drag_offset_.y = my - pos_.y;
                }
            }
        }
        else if (e.type == Input::Event::MOUSE_UP)
        {
            dragging_ = false;
        }
    }
    if (dragging_ && type_ == DRAGGABLE)
    {
        // Update position
        set_pos(mx - drag_offset_.x, my - drag_offset_.y);

        // Keep window in bounds
        Box screen = gui_->screen_dimensions();
        if (pos_.x < 0)
        {
            pos_.x = 0;
        }
        if (pos_.x + pos_.w > screen.w)
        {
            pos_.x = screen.w - pos_.w;
        }
        if (pos_.y < 0)
        {
            pos_.y = 0;
        }
        if (pos_.y + pos_.h > screen.h)
        {
            pos_.y = screen.h - pos_.h;
        }
        input_handled = true;
    }

    // Handle input for window controls
    for (auto& c : controls_)
    {
        if (!c->hidden())
        {
            input_handled |= c->Update(input);
        }
    }
    return input_handled;
}

Button* Window::MakeButton(units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string label)
{
    Box pos(x, y, width, height);
    std::unique_ptr<Button> button(new Button(pos, label, gui_, this));
    controls_.push_back(std::move(button));
    return static_cast<Button*>(controls_.back().get());
}

Image* Window::MakeImage(units::pixel x, units::pixel y, units::pixel width, units::pixel height, std::string filename)
{
    Box pos(x, y, width, height);
    std::unique_ptr<Image> image(new Image(pos, filename, gui_, this));
    controls_.push_back(std::move(image));
    return static_cast<Image*>(controls_.back().get());
}

Image* Window::MakeImage(units::pixel x, units::pixel y, units::pixel width, units::pixel height, const PixelData& pixel_data)
{
    Box pos(x, y, width, height);
    std::unique_ptr<Image> image(new Image(pos, pixel_data, gui_, this));
    controls_.push_back(std::move(image));
    return static_cast<Image*>(controls_.back().get());
}

DebugSlider* Window::MakeDebugSlider(units::pixel x, units::pixel y, units::pixel width, units::pixel height, float min, float max, float step)
{
    Box pos(x, y, width, height);
    std::unique_ptr<DebugSlider> slider(new DebugSlider(pos, min, max, step, gui_, this));
    controls_.push_back(std::move(slider));
    return static_cast<DebugSlider*>(controls_.back().get());
}

Label* Window::MakeLabel(units::pixel x, units::pixel y, std::string text)
{
    Vector2 pos(units::pixel_to_subpixel(x),
                units::pixel_to_subpixel(y));
    Vector4 colour = gui_->skin()->layout()->window.colour;
    std::unique_ptr<Label> label(new Label(pos, ColourString(text, colour), gui_, this));
    controls_.push_back(std::move(label));
    return static_cast<Label*>(controls_.back().get());
}

Textarea* Window::MakeTextarea(units::pixel x, units::pixel y, units::pixel width, units::pixel height)
{
    Box pos(x, y, width, height);
    std::unique_ptr<Textarea> textarea(new Textarea(pos, gui_, this));
    controls_.push_back(std::move(textarea));
    return static_cast<Textarea*>(controls_.back().get());
}

Textbox* Window::MakeTextbox(units::pixel x, units::pixel y, units::pixel width, units::pixel height)
{
    Box pos(x, y, width, height);
    std::unique_ptr<Textbox> textbox(new Textbox(pos, gui_, this));
    controls_.push_back(std::move(textbox));
    return static_cast<Textbox*>(controls_.back().get());
}
} // namespace gui
} // namespace blons
