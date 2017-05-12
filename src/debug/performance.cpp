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

#include <blons/debug/performance.h>

// Includes
#include <functional>
#include <set>

namespace blons
{
namespace performance
{
namespace
{
// Contains a list of all actively running performance::Frames
// They add & remove themselves from this list via the
// Frame::Start() and Frame::End() methods
std::set<Frame*> g_active_frames;
} // namespace

void PushMarker(const std::string& label)
{
    // Save ourselves a costly GPU buffer allocation if we can
    if (g_active_frames.size() == 0)
    {
        return;
    }

    Marker marker;
    marker.label = label;
    marker.ready = false;
    marker.gpu_start.reset(render::context()->RegisterTimestamp());
    marker.cpu_timer.start();

    // Push the new Marker to all currently subscribed Frames
    for (auto& frame : g_active_frames)
    {
        frame->active_stack_->child_nodes.push_back(marker);
        auto& node = frame->active_stack_->child_nodes.back();
        node.parent_ = frame->active_stack_;
        frame->active_stack_ = &node;
    }
}

void PopMarker()
{
    // Save ourselves a costly GPU buffer allocation if we can
    if (g_active_frames.size() == 0)
    {
        return;
    }

    auto gpu_end = std::shared_ptr<TimerResource>(render::context()->RegisterTimestamp());

    // Find the top of each frame's stack, stop their timers and set
    // the stack top to the Marker below the active one (its parent)
    for (auto& frame : g_active_frames)
    {
        if (frame->active_stack_->parent_ == nullptr)
        {
            throw "PopMarker called when an active frame is at 0 tree depth";
        }
        auto& node = frame->active_stack_;
        node->cpu_timer.pause();
        node->gpu_end = gpu_end;
        frame->active_stack_ = node->parent_;
    }
}

Frame::Frame()
{
    active_ = false;
}

Frame::~Frame()
{
    End();
}

void Frame::Start()
{
    if (!active_)
    {
        g_active_frames.insert(this);
    }
    active_stack_ = &root_;
    active_ = true;

    // Do the Marker init manually instead of using PushMarker
    // as we don't want the root node being pushed onto other
    // actively running frames
    root_.label = "Frame";
    root_.ready = false;
    root_.cpu_duration = 0;
    root_.gpu_duration = 0;
    root_.gpu_start.reset(render::context()->RegisterTimestamp());
    root_.cpu_timer = Timer();
    root_.parent_ = nullptr;
    root_.child_nodes.clear();
}

void Frame::End()
{
    if (active_)
    {
        g_active_frames.erase(this);
        if (active_stack_->parent_ != nullptr)
        {
            throw "Frame::end() called with an unbalanced Marker tree";
        }
        root_.gpu_end.reset(render::context()->RegisterTimestamp());
        root_.cpu_timer.pause();
    }
    active_ = false;
}

const Marker& Frame::root()
{
    if (active_)
    {
        throw "Frame::root() called while frame is still active";
    }
    uint64_t loop_count = 0;
    while (!root_.ready)
    {
        QueryGPUTimers();
        loop_count++;
    }
    if (loop_count > 1)
    {
        log::Warn("Frame::root() stalled GPU pipeline waiting for result\n");
    }

    return root_;
}

void Frame::QueryGPUTimers()
{
    // No need to query frames we have already completed
    if (root_.ready)
    {
        return;
    }
    auto context = render::context();
    // 0us means the timestamp is not ready yet
    if (context->GetTimestamp(root_.gpu_end.get()) != 0)
    {
        // Recursively iterate through the frame's timers, marking them as ready and retrieving their duration
        std::function<void(Marker*)> recursive_gpu_query = [&](Marker* marker)
        {
            units::time::us gpu_start = context->GetTimestamp(marker->gpu_start.get());
            units::time::us gpu_end = context->GetTimestamp(marker->gpu_end.get());
            marker->gpu_duration = gpu_end - gpu_start;
            marker->cpu_duration = marker->cpu_timer.us();
            marker->ready = true;
            for (auto& child : marker->child_nodes)
            {
                recursive_gpu_query(&child);
            }
        };
        recursive_gpu_query(&root_);
    }
}
} // namespace performance
} // namespace blons