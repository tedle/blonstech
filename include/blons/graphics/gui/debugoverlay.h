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

#ifndef BLONSTECH_GRAPHICS_GUI_DEBUGOVERLAY_H_
#define BLONSTECH_GRAPHICS_GUI_DEBUGOVERLAY_H_

// Includes
#include <unordered_map>
// Public Includes
#include <blons/graphics/gui/control.h>
#include <blons/graphics/gui/overlay.h>
#include <blons/graphics/gui/label.h>
#include <blons/debug/performance.h>

namespace blons
{
namespace gui
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Screen overlay that provides debug information. Currently includes
/// detailed frame timing and a framerate counter
////////////////////////////////////////////////////////////////////////////////
class DebugOverlay : public Overlay
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \copydoc Overlay(Manager*)
    ////////////////////////////////////////////////////////////////////////////////
    DebugOverlay(Manager* parent_manager);
    ~DebugOverlay() {}

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Submits a drawbatch with mesh data & shader inputs to the parent
    /// gui::Manager for itself and each child Control
    ////////////////////////////////////////////////////////////////////////////////
    void Render() override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Performs no input logic
    ///
    /// \param input Handle to the current input events
    /// \return True if input is consumed by Control%s (it's not)
    ////////////////////////////////////////////////////////////////////////////////
    bool Update(const Input& input) override;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Updates displayed performance timings, can be run asynchronously
    ///
    /// \param root_marker Detailed frame timing information
    /// \param frame_time Total frame time in microseconds
    ////////////////////////////////////////////////////////////////////////////////
    void UpdateMetrics(const performance::Marker& root_marker, units::time::us frame_time);

private:
    struct PerfTime
    {
        PerfTime() : cpu_time(0), gpu_time(0) {}
        double cpu_time;
        double gpu_time;
    };
    struct MaxPerfTime : public PerfTime
    {
        MaxPerfTime() : cpu_frame_age(0), gpu_frame_age(0) {}
        int cpu_frame_age;
        int gpu_frame_age;
    };
    struct LabeledBackdrop
    {
        std::unique_ptr<Label> label;
        std::unique_ptr<Image> backdrop;
    };

    void UpdateMaxPerfTimes(const performance::Marker& root_marker);
    void UpdateFpsText(units::time::us frame_time);
    void UpdatePerfText(const performance::Marker& root_marker);

    static constexpr int kAverageFrames = 50; // number of frames to average performance values over
    static constexpr double kFrameWeight = 1.0 / kAverageFrames;

    LabeledBackdrop fps_ui_;
    double total_frame_time_;

    LabeledBackdrop perf_ui_;
    std::unordered_map<std::string, PerfTime> average_perf_times_;
    MaxPerfTime max_perf_times_;
};
} // namepsace GUI
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \class blons::gui::DebugOverlay
/// \ingroup gui
///
/// ### Example:
/// \code
/// blons::performance::Frame frame;
/// auto debug_overlay = gui->MakeOverlay<blons::gui::DebugOverlay>();
/// while (true)
/// {
///     // Start performance profiling frame
///     frame.Start();
///
///     // Render the scene
///     blons::performance::PushMarker("Scene Rendering");
///     // RenderScene() ...
///     blons::performance::PopMarker();
///
///     // Render the GUI
///     blons::performance::PushMarker("User Interface");
///     gui->Render();
///     blons::performance::PopMarker();
///
///     // End performance profiling frame
///     frame.End();
///
///     // Update the debug overlay metrics
///     debug_overlay->UpdateMetrics(frame.root(), frame.root().cpu_duration);
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_GRAPHICS_GUI_DEBUGOVERLAY_H_
