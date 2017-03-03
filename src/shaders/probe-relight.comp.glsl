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
uniform SHColourCoeffs sh_sky_colour;
uniform float sky_luminance;
uniform float temp_ambient;

layout(std430) buffer probe_buffer
{
    Probe probes[];
};

void ComputeAmbientCubeDirection(const uint probe_id, const int cube_face, const vec3 direction)
{
    float direction_coeffs[9];
    SHProjectCosineLobe3(direction, direction_coeffs);
    // Divide by pi because sky_vis is merely a visibility function and is not meant to be scaled for irradiance
    // The irradiance scaling is applied on the sky_light itself
    float sky_vis = max(SHDot3(probes[probe_id].sh_coeffs, direction_coeffs), 0.0f) / kPi;
    vec3 sky_light = vec3(SHDot3(direction_coeffs, sh_sky_colour.r),
                          SHDot3(direction_coeffs, sh_sky_colour.g),
                          SHDot3(direction_coeffs, sh_sky_colour.b));
    // Scale sky irradiance by visibility function
    sky_light *= sky_vis;
    sky_light *= sky_luminance;
    vec3 ambient_light = vec3(temp_ambient) + sky_light;
    probes[probe_id].cube_coeffs[cube_face][0] = ambient_light.r;
    probes[probe_id].cube_coeffs[cube_face][1] = ambient_light.g;
    probes[probe_id].cube_coeffs[cube_face][2] = ambient_light.b;
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