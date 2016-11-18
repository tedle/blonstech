////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
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

#include <blons/graphics/gui/debugslider.h>

// Includes
#include <algorithm>
// Public Includes
#include <blons/graphics/gui/gui.h>
#include "debugsliderbutton.h"
#include "debugslidertextbox.h"

namespace blons
{
namespace gui
{
namespace
{
static const blons::units::pixel kButtonWidth = 25;
static const std::size_t kMaxPrecision = 5;
static const float kMaxPrecisionCheck  = 0.00001f;
static const float kMaxPrecisionBuffer = 0.000001f;

std::size_t FloatPrecision(float x)
{
    std::size_t count = 0;
    x = abs(x);
    while (x - static_cast<int>(x + kMaxPrecisionBuffer) > kMaxPrecisionCheck && count < kMaxPrecision)
    {
        x -= static_cast<int>(x + kMaxPrecisionBuffer);
        x *= 10;
        count++;
    }
    return count;
}
} // namespace

DebugSlider::DebugSlider(Box pos, float min, float max, float step, Manager* parent_manager, Window* parent_window)
    : Control(pos, parent_manager, parent_window)
{
    // Empty lambda is easier than worrying about nullptrs
    callback_ = [](float v){};
    min_ = std::min(min, max);
    max_ = std::max(min, max);
    step_ = std::min(step, max);

    // Determines how many decimal places are printed in slider (purely aesthetic)
    precision_ = FloatPrecision(min_);
    precision_ = std::max(FloatPrecision(max_), precision_);
    precision_ = std::max(FloatPrecision(step_), precision_);
    assert(precision_ <= kMaxPrecision);

    Box textbox_pos = pos;
    textbox_pos.w = pos.w - kButtonWidth;
    textbox_.reset(new DebugSliderTextbox(textbox_pos, Skin::FontStyle::LABEL, parent_manager, parent_window));
    textbox_->set_callback([=](Textbox* textbox)
    {
        // std::stof will throw when supplied garbage data from user
        try
        {
            this->set_value(std::stof(textbox->text()));
        }
        catch (...)
        {
            this->set_progress(0);
        }
        textbox->set_focus(false);
    });

    Box button_pos = pos;
    button_pos.x = pos.x + pos.w - kButtonWidth;
    button_pos.w = kButtonWidth;
    button_.reset(new DebugSliderButton(button_pos, parent_manager, parent_window));
    button_->set_callback([=](units::pixel x_delta, units::pixel y_distance)
    {
        // Precision improves 10 fold every 100 pixels above/below button
        double magnitude = pow(10.0, y_distance / 100.0);
        // Maximize precision at 1 step per 10 pixels
        double progress_per_step = 1.0 / ((max_ - min_) / step_);
        double progress_cap = progress_per_step / 10.0 * abs(x_delta);
        // Apply precision cap unless default (magnitude == 1) is more precise
        double default_precision = abs(x_delta) / textbox_pos.w;
        double scaled_precision = default_precision / magnitude;
        double progress = std::min(default_precision, std::max(progress_cap, scaled_precision));
        // Adjust progress to match sign of x_delta
        progress *= (x_delta > 0 ? 1.0 : -1.0);

        set_progress(this->progress() + progress);
    });

    set_progress(0);
}

DebugSlider::~DebugSlider()
{
}

void DebugSlider::Render()
{
    button_->Render();
    textbox_->Render();
}

bool DebugSlider::Update(const Input& input)
{
    bool input_handled = false;
    input_handled |= textbox_->Update(input);
    input_handled |= button_->Update(input);
    textbox_->set_progress(progress());
    return input_handled;
}

void DebugSlider::set_callback(std::function<void(float)> callback)
{
    callback_ = callback;
}

double DebugSlider::progress() const
{
    return progress_;
}

// Setting progress_ and value_ are both done in the same function since
// the 2 values must be kept synchronized but cannot mutually call each other
void DebugSlider::set_progress(double progress)
{
    // Set progress_
    progress_ = std::max(0.0, std::min(progress, 1.0));

    // Set value_
    float p = static_cast<float>(progress_);
    float value = (max_ - min_) * p + min_;

    // Round to nearest step
    float step_offset = std::fmod(value - min_, step_);
    // Account for max being above nearest divisible step
    if (progress_ == 1.0)
    {
        value = max_;
    }
    else if (step_offset > step_ / 2.0f)
    {
        value += step_ - step_offset;
    }
    else
    {
        value -= step_offset;
    }
    // Account for max being below nearest divisible step
    value = std::min(max_, std::max(min_, value));

    // Call user supplied function if value has changed
    if (value != value_)
    {
        callback_(value);
    }
    // Update value
    value_ = value;

    // Update textbox
    // Wonky ternary add to hide float imprecision when printing
    auto pretty_print_offset = (value_ >= -kMaxPrecisionBuffer ? kMaxPrecisionCheck : -kMaxPrecisionCheck);
    auto value_str = std::to_string(value_ + pretty_print_offset);
    // Truncate to proper precision
    auto decimal_place = value_str.find(".");
    if (decimal_place != std::string::npos)
    {
        // Cut off an extra character for integers to account for the actual decimal point
        bool is_int = (precision_ == 0);
        value_str = value_str.substr(0, decimal_place + precision_ + (is_int ? 0 : 1));
    }
    textbox_->set_text(value_str);
}

float DebugSlider::value() const
{
    return value_;
}

void DebugSlider::set_value(float value)
{
    set_progress((value - min_) / (max_ - min_));
}
} // namespace gui
} // namespace blons
