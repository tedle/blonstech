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
#include <shaders/lib/shadow.lib.glsl>
#include <shaders/lib/probes.lib.glsl>

// Workgroup size
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// Globals
uniform mat4 light_vp_matrix;
uniform sampler2D light_depth;
uniform DirectionalLight sun;
uniform vec3 metalness;
uniform float gi_boost;

vec3 ComputeSurfelLighting(inout Surfel surfel)
{
    vec3 radiance;
    // Extract and rebuild surfel data as needed
    vec4 pos = vec4(surfel.pos[0], surfel.pos[1], surfel.pos[2], 1.0);
    // Check if the surfel exists within the light's frustum, pretty homogenous I think (hope)
    if (IsValidShadowSample(pos, light_vp_matrix))
    {
        vec3 normal = vec3(surfel.normal[0], surfel.normal[1], surfel.normal[2]);
        vec3 albedo = vec3(surfel.albedo[0], surfel.albedo[1], surfel.albedo[2]);
        float NdotL = max(dot(normal, -sun.dir), 0.0);
        float light_visibility = ShadowTest(pos, light_vp_matrix, light_depth);

        // We don't use a PBR diffuse term since those require a view vector which does not exist here
        radiance = light_visibility * sun.luminance * sun.colour * NdotL * gi_boost;
        // Use previous frame's ambient term of nearest probe to approximate infinite bounce lighting
        Probe nearest_probe = FindProbe(surfel.nearest_probe_id);
        vec3 ambient_cube[6] = vec3[6](
            vec3(nearest_probe.cube_coeffs[kPositiveX][0], nearest_probe.cube_coeffs[kPositiveX][1], nearest_probe.cube_coeffs[kPositiveX][2]),
            vec3(nearest_probe.cube_coeffs[kNegativeX][0], nearest_probe.cube_coeffs[kNegativeX][1], nearest_probe.cube_coeffs[kNegativeX][2]),
            vec3(nearest_probe.cube_coeffs[kPositiveY][0], nearest_probe.cube_coeffs[kPositiveY][1], nearest_probe.cube_coeffs[kPositiveY][2]),
            vec3(nearest_probe.cube_coeffs[kNegativeY][0], nearest_probe.cube_coeffs[kNegativeY][1], nearest_probe.cube_coeffs[kNegativeY][2]),
            vec3(nearest_probe.cube_coeffs[kPositiveZ][0], nearest_probe.cube_coeffs[kPositiveZ][1], nearest_probe.cube_coeffs[kPositiveZ][2]),
            vec3(nearest_probe.cube_coeffs[kNegativeZ][0], nearest_probe.cube_coeffs[kNegativeZ][1], nearest_probe.cube_coeffs[kNegativeZ][2])
        );
        vec3 ambient_light = SampleAmbientCube(ambient_cube, normal);
        radiance += ambient_light;
        // Attenuate by surface colour
        radiance *= albedo;
        // Metals dont have diffuse light
        radiance *= 1.0 - metalness;
        // We also divide by pi now since we are storing radiance, not irradiance
        radiance /= kPi;
        surfel.radiance[0] = radiance.r;
        surfel.radiance[1] = radiance.g;
        surfel.radiance[2] = radiance.b;
    }
    // If we don't have a valid shadow sample, re-use old data
    else
    {
        radiance = vec3(surfel.radiance[0], surfel.radiance[1], surfel.radiance[2]);
    }
    return radiance;
}

void main(void)
{
    // Read the entire brick from SSBO
    uint brick_id = gl_GlobalInvocationID.x;
    SurfelBrick brick = FindProbeSurfelBrick(brick_id);

    vec3 radiance = vec3(0);
    // Update lighting of every surfel in this brick while building a radiance term
    for (int surfel_id = brick.surfel_range_start; surfel_id < brick.surfel_range_start + brick.surfel_count; surfel_id++)
    {
        // Read surfel
        Surfel surfel = FindProbeSurfel(surfel_id);
        // Update lighting and build radiance term
        radiance += ComputeSurfelLighting(surfel);
        // Update surfel
        SetProbeSurfel(surfel_id, surfel);
    }
    // Average radiance
    radiance /= float(brick.surfel_count);
    // Convert to brick format
    brick.radiance[0] = radiance.r;
    brick.radiance[1] = radiance.g;
    brick.radiance[2] = radiance.b;
    // Update brick
    SetProbeSurfelBrick(brick_id, brick);
}