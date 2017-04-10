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
uniform SHColourCoeffs sh_sky_colour;
uniform float sky_luminance;

struct AmbientCubeDirection
{
    int face;
    vec3 direction;
};

const AmbientCubeDirection kBasisDirections[6] = AmbientCubeDirection[6](
    AmbientCubeDirection(kPositiveX, vec3( 1.0,  0.0,  0.0)),
    AmbientCubeDirection(kNegativeX, vec3(-1.0,  0.0,  0.0)),
    AmbientCubeDirection(kPositiveY, vec3( 0.0,  1.0,  0.0)),
    AmbientCubeDirection(kNegativeY, vec3( 0.0, -1.0,  0.0)),
    AmbientCubeDirection(kPositiveZ, vec3( 0.0,  0.0,  1.0)),
    AmbientCubeDirection(kNegativeZ, vec3( 0.0,  0.0, -1.0))
);

void ComputeAmbientCube(const uint probe_id)
{
    Probe probe = FindProbe(probe_id);
    // Calculate sky lighting in all 6 basis directions
    for (int dir = 0; dir < 6; dir++)
    {
        const int cube_face = kBasisDirections[dir].face;
        const vec3 direction = kBasisDirections[dir].direction;

        float direction_coeffs[9];
        SHProjectCosineLobe3(direction, direction_coeffs);
        // Divide by pi because sky_vis is merely a visibility function and is not meant to be scaled for irradiance
        // The irradiance scaling is applied on the sky_light itself
        float sky_vis = max(SHDot3(probe.sh_coeffs, direction_coeffs), 0.0f) / kPi;
        vec3 sky_light = vec3(
            SHDot3(direction_coeffs, sh_sky_colour.r),
            SHDot3(direction_coeffs, sh_sky_colour.g),
            SHDot3(direction_coeffs, sh_sky_colour.b)
        );
        // Scale sky irradiance by visibility function
        sky_light *= sky_vis;
        sky_light *= sky_luminance;
        vec3 ambient_light = sky_light;

        // Store as ambient cube coefficients
        probe.cube_coeffs[cube_face][0] = ambient_light.r;
        probe.cube_coeffs[cube_face][1] = ambient_light.g;
        probe.cube_coeffs[cube_face][2] = ambient_light.b;
    }

    // Iterate over brick factors to sum up bounce irradiance
    // Separate loop to cut down on global memory reads
    for (int factor_id = probe.brick_factor_range_start;
        factor_id < probe.brick_factor_range_start + probe.brick_factor_count;
        factor_id++)
    {
        SurfelBrickFactor factor = FindProbeSurfelBrickFactor(factor_id);
        SurfelBrick brick = FindProbeSurfelBrick(factor.brick_id);
        vec3 radiance = vec3(brick.radiance[0], brick.radiance[1], brick.radiance[2]);
        for (int dir = 0; dir < 6; dir++)
        {
            const int cube_face = kBasisDirections[dir].face;
            // Brick weights sum up to pi over the set of factors
            vec3 weighted_radiance = radiance * factor.brick_weights[cube_face];
            // Add to ambient cube coefficients as we go
            probe.cube_coeffs[cube_face][0] += weighted_radiance.r;
            probe.cube_coeffs[cube_face][1] += weighted_radiance.g;
            probe.cube_coeffs[cube_face][2] += weighted_radiance.b;
        }
    }
    FindProbe(probe_id).cube_coeffs = probe.cube_coeffs;
}

void main(void)
{
    uint probe_id = gl_GlobalInvocationID.x;

    // Sample the probe's sky vis coefficients
    // Sample brick radiance
    // Store as an ambient cube
    ComputeAmbientCube(probe_id);
}