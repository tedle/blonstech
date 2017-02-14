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
#include <shaders/probe.lib.glsl>
#include <shaders/sh-math.lib.glsl>

// Workgroup size
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// Globals
layout(rgba32f, binding = 0) writeonly uniform image3D irradiance_volume_px_nx_py_ny_out;
layout(rg32f, binding = 1) writeonly uniform image3D irradiance_volume_pz_nz_out;
uniform mat4 world_matrix;

layout(std430) buffer probe_buffer
{
    Probe probes[];
};

void main(void)
{
    // Figure out the world space coordinates of this irradiance sample
    vec3 texels = vec3(gl_NumWorkGroups * gl_WorkGroupSize);
    // Offset by half a texel so our samples represent the same area they are stored at
    vec3 sample_pos = (vec3(gl_GlobalInvocationID) + vec3(0.5f)) / texels;
    vec4 world_pos = world_matrix * vec4(sample_pos, 1.0);

    // Find the probe nearest to this sample
    Probe nearest_probe = probes[0];
    float nearest_length = distance(world_pos.xyz, vec3(nearest_probe.pos[0], nearest_probe.pos[1], nearest_probe.pos[2]));
    for (int i = 1; i < probes.length(); i++)
    {
        float probe_length = distance(world_pos.xyz, vec3(probes[i].pos[0], probes[i].pos[1], probes[i].pos[2]));
        if (probe_length < nearest_length)
        {
            nearest_probe = probes[i];
            nearest_length = probe_length;
        }
    }

    // Sample the nearest probe's sky vis coefficients in each principle normal direction
    // Store as an ambient cube
    // TODO: This will eventually be done per-probe instead of per-voxel
    float ambient_cube[6];
    float direction_coeffs[9];

    // +X
    SHProjectDirection3(vec3(1.0, 0.0, 0.0), direction_coeffs);
    ambient_cube[0] = max(SHDot3(nearest_probe.sh_coeffs, direction_coeffs), 0.0f);
    // -X
    SHProjectDirection3(vec3(-1.0, 0.0, 0.0), direction_coeffs);
    ambient_cube[1] = max(SHDot3(nearest_probe.sh_coeffs, direction_coeffs), 0.0f);
    // +Y
    SHProjectDirection3(vec3(0.0, 1.0, 0.0), direction_coeffs);
    ambient_cube[2] = max(SHDot3(nearest_probe.sh_coeffs, direction_coeffs), 0.0f);
    // -Y
    SHProjectDirection3(vec3(0.0, -1.0, 0.0), direction_coeffs);
    ambient_cube[3] = max(SHDot3(nearest_probe.sh_coeffs, direction_coeffs), 0.0f);
    // +Z
    SHProjectDirection3(vec3(0.0, 0.0, 1.0), direction_coeffs);
    ambient_cube[4] = max(SHDot3(nearest_probe.sh_coeffs, direction_coeffs), 0.0f);
    // -Z
    SHProjectDirection3(vec3(0.0, 0.0, -1.0), direction_coeffs);
    ambient_cube[5] = max(SHDot3(nearest_probe.sh_coeffs, direction_coeffs), 0.0f);

    imageStore(irradiance_volume_px_nx_py_ny_out, ivec3(gl_GlobalInvocationID),
               vec4(ambient_cube[0], ambient_cube[1], ambient_cube[2], ambient_cube[3]));
    imageStore(irradiance_volume_pz_nz_out, ivec3(gl_GlobalInvocationID),
               vec4(ambient_cube[4], ambient_cube[5], 0.0f, 0.0f));
}