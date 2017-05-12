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

#ifndef BLONSTECH_DEBUG_PERFORMANCE_H_
#define BLONSTECH_DEBUG_PERFORMANCE_H_

// Includes
#include <string>
#include <vector>
// Public Includes
#include <blons/graphics/render/renderer.h>
#include <blons/math/units.h>
#include <blons/system/timer.h>

namespace blons
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Functions and classes for performance profiling CPU and GPU work in a
/// given frame.
////////////////////////////////////////////////////////////////////////////////
namespace performance
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Adds a new range marker to the top of the active stack for all
/// currently running Frame%s.
///
/// \param label Label describing the new profile range
////////////////////////////////////////////////////////////////////////////////
void PushMarker(const std::string& label);
////////////////////////////////////////////////////////////////////////////////
/// \brief Marks the end of the current profile range. Will throw if called
/// while the stack of any active Frame is currently pointing to its root node
////////////////////////////////////////////////////////////////////////////////
void PopMarker();

////////////////////////////////////////////////////////////////////////////////
/// \brief Contains information for marked profiling ranges
////////////////////////////////////////////////////////////////////////////////
struct Marker
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Text label describing the marked range
    ////////////////////////////////////////////////////////////////////////////////
    std::string label;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Execution time on the CPU for the marked range
    ////////////////////////////////////////////////////////////////////////////////
    units::time::us cpu_duration = 0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Execution time on the GPU for the marked range
    ////////////////////////////////////////////////////////////////////////////////
    units::time::us gpu_duration = 0;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief List of nested ranges
    ////////////////////////////////////////////////////////////////////////////////
    std::vector<Marker> child_nodes;

private:
    friend class Frame;
    friend void PushMarker(const std::string& label);
    friend void PopMarker();

    bool ready = false;
    std::shared_ptr<TimerResource> gpu_start;
    std::shared_ptr<TimerResource> gpu_end;
    Timer cpu_timer;
    Marker* parent_;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Collects profile range markers created by global
/// performance::PushMarker(const std::string&) and performance::PopMarker()
/// calls while active. For detailed usage examples see the blons::performance
/// page
////////////////////////////////////////////////////////////////////////////////
class Frame
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new idle Frame as well as a root Marker that is not pushed
    /// onto the global stack.
    ////////////////////////////////////////////////////////////////////////////////
    Frame();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Destroys the Frame, forcing a call to Frame::End() and removing it
    /// from the list of active Frame%s.
    ////////////////////////////////////////////////////////////////////////////////
    ~Frame();
    Frame(const Frame&) = delete;
    Frame& operator= (const Frame&) = delete;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Starts recording range markers for the current Frame. If the Frame is
    /// already active, all internal state will be reset and started again.
    ////////////////////////////////////////////////////////////////////////////////
    void Start();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Marks the current Frame as inactive, removing it from the list of
    /// active Frame%s, and allowing the retrieval of the root Marker node. Will
    /// throw if there are still open-ended Marker ranges on the stack.
    ////////////////////////////////////////////////////////////////////////////////
    void End();
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the root Marker for the Frame. This is a blocking operation
    /// and will wait until the GPU workload has caught up to all requested
    /// timestamps before returning. To prevent GPU pipeline stalls it is
    /// recommended to only use this method once 3 or more rendering frames have
    /// passed. Will throw if called while the Frame is still active.
    ///
    /// \return Root Marker for the requested frame
    ////////////////////////////////////////////////////////////////////////////////
    const Marker& root();

private:
    friend void PushMarker(const std::string& label);
    friend void PopMarker();

    void QueryGPUTimers();

    Marker root_;
    Marker* active_stack_; // Marker to Push/Pop from
    bool active_; // True when between Start()/End() calls
};
} // namespace performance
} // namespace blons

////////////////////////////////////////////////////////////////////////////////
/// \namespace blons::performance
/// \ingroup debug
///
/// ### Example:
/// \code
/// const std::size_t kBufferSize = 3;
/// std::array<blons::performance::Frame, kBufferSize> frames;
/// int active_frame = 0;
/// while (1)
/// {
///     // Start profiling a new frame
///     auto& frame = frames[active_frame];
///     frame.Start();
///
///     blons::performance::PushMarker("Geometry");
///     // Geometry rendering...
///     blons::performance::PopMarker();
///
///     blons::performance::PushMarker("Shadows");
///     // Shadow maps...
///     blons::performance::PopMarker();
///
///     // Mark the end of the profile range
///     frame.End();
///
///     // Retrieve the root Marker from (kBufferSize - 1) frames ago
///     // Will stall the pipeline if GPU work hasn't completed, so a ring buffer size of 3 or more is recommended
///     auto root = frames[(active_frame + kBufferSize - 1) % kBufferSize].root();
///     blons::log::Debug("Frame took %ius CPU time and %ius GPU time\n", root.cpu_duration, root.gpu_duration);
///     for (const auto& marker : root.child_nodes)
///     {
///         blons::log::Debug("\t%s took %ius CPU time and %ius GPU time\n", marker.label.c_str(), marker.cpu_duration, marker.gpu_duration);
///     }
///     active_frame = (active_frame + 1) % kBufferSize;
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////

#endif // BLONSTECH_DEBUG_PERFORMANCE_H_