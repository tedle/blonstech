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

#include <blons/graphics/gui/debugoverlay.h>

// Includes
#include <functional>
#include <iomanip>
#include <sstream>
// Public Includes
#include <blons/graphics/gui/gui.h>

namespace blons
{
namespace gui
{
namespace
{
const Skin::FontStyle kPerfBreakdownFont = Skin::FontStyle::DEBUG;

Vector3 LowDiscrepancyColour(int index)
{
    // Lazy, fast, deterministic, PRNGish
    auto sample = FastHash(&index, sizeof(index));
    auto sample_max = std::numeric_limits<decltype(sample)>::max();
    float uniform = static_cast<float>(sample) / static_cast<float>(sample_max);

    float hue = VanDerCorputSequence(index);
    float saturation = uniform * 0.4f + 0.5f;
    return HsvToRgb(Vector3(hue, saturation, 1.0f));
}

std::unique_ptr<Image> BuildColourImage(Vector4 colour, Manager* gui, Window* window)
{
    PixelData p;
    p.width = 1;
    p.height = 1;
    p.type = TextureType(TextureType::R8G8B8A8, TextureType::RAW, TextureType::LINEAR, TextureType::CLAMP);
    p.pixels.push_back(static_cast<unsigned char>(colour.r * 255.0f));
    p.pixels.push_back(static_cast<unsigned char>(colour.g * 255.0f));
    p.pixels.push_back(static_cast<unsigned char>(colour.b * 255.0f));
    p.pixels.push_back(static_cast<unsigned char>(colour.a * 255.0f));
    return std::make_unique<Image>(Box(0, 0, 0, 0), p, gui, window);
}
} // namespace

DebugOverlay::DebugOverlay(std::string id, Box pos, std::string caption, Type type, Manager* parent_manager)
    : Window(id, pos, caption, type, parent_manager)
{
    auto layout = gui_->skin()->layout();
    perf_ui_.label.reset(new Label(Vector2(), ColourString("", layout->debug_overlay.default_colour), kPerfBreakdownFont, parent_manager, this));
    perf_ui_.backdrop = BuildColourImage(layout->debug_overlay.backdrop_colour, gui_, this);
    hidden_ = false;
}

void DebugOverlay::Render()
{
    perf_ui_.backdrop->Render();
    perf_ui_.label->Render();
    Window::Render();
}

void DebugOverlay::UpdateMetrics(const performance::Marker& root_marker)
{
    // For max() frame times of the previous kAverageFrames
    UpdateMaxPerfTimes(root_marker);

    // Precalculate some constants for small efficiency gain
    const auto& layout = gui_->skin()->layout();
    const std::string kBaseColourCode = ColourString::MakeColourCode(layout->debug_overlay.default_colour);
    const std::string kCpuColourCode = ColourString::MakeColourCode(layout->debug_overlay.cpu_colour);
    const std::string kGpuColourCode = ColourString::MakeColourCode(layout->debug_overlay.gpu_colour);
    const std::size_t kMaxLabelWidth = 35;
    std::size_t kMaxLineWidth = kMaxLabelWidth + std::string(">0.00ms CPU 0.00ms GPU").length();
    int line_count = 0;

    // Builds heading string of perf timings including current frame, avg, max times
    // Formatted as:
    // Performance timing (sample of kAverageFrames frames):
    // CPU 0.00ms avg:0.00ms max:0.00ms
    // GPU 0.00ms avg:0.00ms max:0.00ms
    std::function<std::string()> build_header_string =
    [&]() -> std::string
    {
        auto& root_perf_time = average_perf_times_[root_marker.label];
        std::stringstream text;
        text << std::setprecision(2) << std::fixed;
        text << kBaseColourCode << "Performance timing (sample of " << kAverageFrames << " frames):" << std::endl
             // CPU timings
             << kCpuColourCode << "CPU " << (root_marker.cpu_duration / 1000.0) << "ms "
             << "avg:" << (root_perf_time.cpu_time / 1000.0) << "ms "
             << "max:" << (max_perf_times_.cpu_time / 1000.0) << "ms" << std::endl
             // GPU timings
             << kGpuColourCode << "GPU " << (root_marker.gpu_duration / 1000.0) << "ms "
             << "avg:" << (root_perf_time.gpu_time / 1000.0) << "ms "
             << "max:" << (max_perf_times_.gpu_time / 1000.0) << "ms" << std::endl << std::endl;
        // number of std::endl in this function
        line_count += 4;
        return text.str();
    };
    // Recursively scans Marker tree, building performance string along the way
    // Formatted as:
    // [                    kMaxLineWidth                ]
    // [       kMaxLabelWidth      ]                     |
    // [depth*2]                   |                     |
    //          MarkerLabel--------->0.00ms CPU 0.00ms GPU
    std::function<std::string(const performance::Marker&,int)> build_marker_string =
    [&](const performance::Marker& marker, int depth) -> std::string
    {
        auto& perf_time = average_perf_times_[marker.label];
        perf_time.cpu_time = perf_time.cpu_time * (1.0 - kFrameWeight) + marker.cpu_duration * kFrameWeight;
        perf_time.gpu_time = perf_time.gpu_time * (1.0 - kFrameWeight) + marker.gpu_duration * kFrameWeight;
        std::stringstream text;
        text << std::string(depth * 2, ' ')
             << kBaseColourCode << std::left << std::setfill('-') << std::setw(kMaxLabelWidth - depth * 2) << marker.label << ">"
             << kCpuColourCode << std::setprecision(2) << std::fixed << (perf_time.cpu_time / 1000.0) << "ms CPU "
             << kGpuColourCode << (perf_time.gpu_time / 1000.0) << "ms GPU"
             << std::endl;
        for (const auto& child : marker.child_nodes)
        {
            text << build_marker_string(child, depth + 1);
        }
        line_count++;
        return text.str();
    };
    // Build the string to be displayed for performance timings
    std::string text = build_header_string() + build_marker_string(root_marker, 0);
    // Update the colour string
    perf_ui_.label->set_text(text);

    // Position the backdrop and perf timing text
    auto font = gui_->skin()->font(kPerfBreakdownFont);
    const units::pixel kBackdropPadding = 15;
    units::pixel line_height = font->line_height();
    units::pixel letter_height = font->letter_height();
    units::pixel line_padding = line_height - letter_height;
    units::pixel backdrop_width = font->string_width(std::string(kMaxLineWidth, ' '), false) + (kBackdropPadding * 2);
    units::pixel backdrop_height = (line_height * line_count) - line_padding + (kBackdropPadding * 2);
    auto dims = gui_->screen_dimensions();
    perf_ui_.backdrop->set_pos(Box(0,
                                   units::subpixel_to_pixel(dims.h) - backdrop_height,
                                   backdrop_width,
                                   backdrop_height));
    perf_ui_.label->set_pos(units::pixel_to_subpixel(kBackdropPadding),
                            dims.h - units::pixel_to_subpixel(backdrop_height) + kBackdropPadding + letter_height);
}

bool DebugOverlay::Update(const Input& input)
{
    return false;
}

void DebugOverlay::UpdateMaxPerfTimes(const performance::Marker& root_marker)
{
    if (max_perf_times_.cpu_frame_age >= kAverageFrames)
    {
        max_perf_times_.cpu_frame_age = 0;
        max_perf_times_.cpu_time = 0;
    }
    if (root_marker.cpu_duration > max_perf_times_.cpu_time)
    {
        max_perf_times_.cpu_time = static_cast<double>(root_marker.cpu_duration);
        max_perf_times_.cpu_frame_age = 0;
    }
    else
    {
        max_perf_times_.cpu_frame_age++;
    }
    if (max_perf_times_.gpu_frame_age >= kAverageFrames)
    {
        max_perf_times_.gpu_frame_age = 0;
        max_perf_times_.gpu_time = 0;
    }
    if (root_marker.gpu_duration > max_perf_times_.gpu_time)
    {
        max_perf_times_.gpu_time = static_cast<double>(root_marker.gpu_duration);
        max_perf_times_.gpu_frame_age = 0;
    }
    else
    {
        max_perf_times_.gpu_frame_age++;
    }
}
} // namespace gui
} // namespace blons