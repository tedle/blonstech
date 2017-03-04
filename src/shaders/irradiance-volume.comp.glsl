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

#version 430

// Includes
#include <shaders/lib/types.lib.glsl>
#include <shaders/lib/math.lib.glsl>

// Workgroup size
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// Globals
layout(rgba16f, binding = 0) writeonly uniform image3D irradiance_volume_px_out;
layout(rgba16f, binding = 1) writeonly uniform image3D irradiance_volume_nx_out;
layout(rgba16f, binding = 2) writeonly uniform image3D irradiance_volume_py_out;
layout(rgba16f, binding = 3) writeonly uniform image3D irradiance_volume_ny_out;
layout(rgba16f, binding = 4) writeonly uniform image3D irradiance_volume_pz_out;
layout(rgba16f, binding = 5) writeonly uniform image3D irradiance_volume_nz_out;
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

    // TODO: Optimize this naive search, it actually hurts performance massively
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

    imageStore(irradiance_volume_px_out, ivec3(gl_GlobalInvocationID), vec4(nearest_probe.cube_coeffs[kPositiveX][0],
                                                                            nearest_probe.cube_coeffs[kPositiveX][1],
                                                                            nearest_probe.cube_coeffs[kPositiveX][2],
                                                                            0.0));
    imageStore(irradiance_volume_nx_out, ivec3(gl_GlobalInvocationID), vec4(nearest_probe.cube_coeffs[kNegativeX][0],
                                                                            nearest_probe.cube_coeffs[kNegativeX][1],
                                                                            nearest_probe.cube_coeffs[kNegativeX][2],
                                                                            0.0));
    imageStore(irradiance_volume_py_out, ivec3(gl_GlobalInvocationID), vec4(nearest_probe.cube_coeffs[kPositiveY][0],
                                                                            nearest_probe.cube_coeffs[kPositiveY][1],
                                                                            nearest_probe.cube_coeffs[kPositiveY][2],
                                                                            0.0));
    imageStore(irradiance_volume_ny_out, ivec3(gl_GlobalInvocationID), vec4(nearest_probe.cube_coeffs[kNegativeY][0],
                                                                            nearest_probe.cube_coeffs[kNegativeY][1],
                                                                            nearest_probe.cube_coeffs[kNegativeY][2],
                                                                            0.0));
    imageStore(irradiance_volume_pz_out, ivec3(gl_GlobalInvocationID), vec4(nearest_probe.cube_coeffs[kPositiveZ][0],
                                                                            nearest_probe.cube_coeffs[kPositiveZ][1],
                                                                            nearest_probe.cube_coeffs[kPositiveZ][2],
                                                                            0.0));
    imageStore(irradiance_volume_nz_out, ivec3(gl_GlobalInvocationID), vec4(nearest_probe.cube_coeffs[kNegativeZ][0],
                                                                            nearest_probe.cube_coeffs[kNegativeZ][1],
                                                                            nearest_probe.cube_coeffs[kNegativeZ][2],
                                                                            0.0));
}