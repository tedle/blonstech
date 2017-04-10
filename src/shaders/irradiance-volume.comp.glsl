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
#include <shaders/lib/probes.lib.glsl>

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

void main(void)
{
    // Figure out the world space coordinates of this irradiance sample
    vec3 texels = vec3(gl_NumWorkGroups * gl_WorkGroupSize);
    // Offset by half a texel so our samples represent the same area they are stored at
    vec3 sample_pos = (vec3(gl_GlobalInvocationID) + vec3(0.5f)) / texels;
    vec4 world_pos = world_matrix * vec4(sample_pos, 1.0);

    ProbeWeight weights[4] = FindProbeWeights(world_pos.xyz);
    Probe probes[4] = { FindProbe(weights[0].id), FindProbe(weights[1].id), FindProbe(weights[2].id), FindProbe(weights[3].id) };
    float cube_coeffs[6][3];
    for (int face = 0; face < 6; face++)
    {
        for (int channel = 0; channel < 3; channel++)
        {
            cube_coeffs[face][channel] =  probes[0].cube_coeffs[face][channel] * weights[0].weight;
            cube_coeffs[face][channel] += probes[1].cube_coeffs[face][channel] * weights[1].weight;
            cube_coeffs[face][channel] += probes[2].cube_coeffs[face][channel] * weights[2].weight;
            cube_coeffs[face][channel] += probes[3].cube_coeffs[face][channel] * weights[3].weight;
        }
    }

    imageStore(irradiance_volume_px_out, ivec3(gl_GlobalInvocationID), vec4(cube_coeffs[kPositiveX][0],
                                                                            cube_coeffs[kPositiveX][1],
                                                                            cube_coeffs[kPositiveX][2],
                                                                            0.0));
    imageStore(irradiance_volume_nx_out, ivec3(gl_GlobalInvocationID), vec4(cube_coeffs[kNegativeX][0],
                                                                            cube_coeffs[kNegativeX][1],
                                                                            cube_coeffs[kNegativeX][2],
                                                                            0.0));
    imageStore(irradiance_volume_py_out, ivec3(gl_GlobalInvocationID), vec4(cube_coeffs[kPositiveY][0],
                                                                            cube_coeffs[kPositiveY][1],
                                                                            cube_coeffs[kPositiveY][2],
                                                                            0.0));
    imageStore(irradiance_volume_ny_out, ivec3(gl_GlobalInvocationID), vec4(cube_coeffs[kNegativeY][0],
                                                                            cube_coeffs[kNegativeY][1],
                                                                            cube_coeffs[kNegativeY][2],
                                                                            0.0));
    imageStore(irradiance_volume_pz_out, ivec3(gl_GlobalInvocationID), vec4(cube_coeffs[kPositiveZ][0],
                                                                            cube_coeffs[kPositiveZ][1],
                                                                            cube_coeffs[kPositiveZ][2],
                                                                            0.0));
    imageStore(irradiance_volume_nz_out, ivec3(gl_GlobalInvocationID), vec4(cube_coeffs[kNegativeZ][0],
                                                                            cube_coeffs[kNegativeZ][1],
                                                                            cube_coeffs[kNegativeZ][2],
                                                                            0.0));
}