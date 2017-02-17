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

#version 430

// Includes
#include <shaders/types.lib.glsl>
#include <shaders/math.lib.glsl>

// Workgroup size
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// Globals
layout(std430) buffer probe_buffer
{
    Probe probes[];
};

void ComputeAmbientCubeDirection(const uint probe_id, const int cube_face, const vec3 direction)
{
    float direction_coeffs[9];
    SHProjectCosineLobe3(direction, direction_coeffs);
    vec3 diffuse_colour = vec3(max(SHDot3(probes[probe_id].sh_coeffs, direction_coeffs), 0.0f));
    probes[probe_id].cube_coeffs[cube_face][0] = diffuse_colour.r;
    probes[probe_id].cube_coeffs[cube_face][1] = diffuse_colour.g;
    probes[probe_id].cube_coeffs[cube_face][2] = diffuse_colour.b;
}

void main(void)
{
    uint probe_id = gl_GlobalInvocationID.x;

    // Sample the probe's sky vis coefficients in each principle normal direction
    // Store as an ambient cube
    // +X
    ComputeAmbientCubeDirection(probe_id, kPositiveX, vec3(1.0, 0.0, 0.0));
    // -X
    ComputeAmbientCubeDirection(probe_id, kNegativeX, vec3(-1.0, 0.0, 0.0));
    // +Y
    ComputeAmbientCubeDirection(probe_id, kPositiveY, vec3(0.0, 1.0, 0.0));
    // -Y
    ComputeAmbientCubeDirection(probe_id, kNegativeY, vec3(0.0, -1.0, 0.0));
    // +Z
    ComputeAmbientCubeDirection(probe_id, kPositiveZ, vec3(0.0, 0.0, 1.0));
    // -Z
    ComputeAmbientCubeDirection(probe_id, kNegativeZ, vec3(0.0, 0.0, -1.0));
}