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
#include <shaders/lib/sky.lib.glsl>

// Ins n outs
in vec2 tex_coord;

out vec4 env_map_colour;
out vec4 ld_term_colour;

// Globals
uniform mat4 inv_direction_matrices[6];
uniform mat4 inv_vp_matrices[6];
uniform mat4 light_vp_matrix;
uniform mat4 inv_irradiance_matrix;
uniform samplerCube albedo;
uniform samplerCube normal;
uniform samplerCube depth;
uniform sampler2D light_depth;
uniform sampler3D irradiance_volume_px;
uniform sampler3D irradiance_volume_nx;
uniform sampler3D irradiance_volume_py;
uniform sampler3D irradiance_volume_ny;
uniform sampler3D irradiance_volume_pz;
uniform sampler3D irradiance_volume_nz;
uniform DirectionalLight sun;
uniform SHColourCoeffs sh_sky_colour;
uniform float sky_luminance;

// This function is mostly copied from shaders/light.frag.glsl
// We've avoided abstracting this away to a library because of some minor changes
// in the way this function accounts for specular normalization. Here it has been
// removed because this pass does not calculate specular lighting in any capacity
vec3 AmbientDiffuse(vec4 pos, vec3 normal)
{
    // Irradiance volume stored as ambient cube, reconstruct indirect lighting from data
    vec4 irradiance_sample_pos = inv_irradiance_matrix * pos;
    irradiance_sample_pos /= irradiance_sample_pos.w;
    vec3 ambient_cube[6];
    // We do a lot of ugly busywork to cut texture fetches down in half because these are dependent texture fetches and wow those are expensive!!!
    bvec3 is_positive = bvec3(normal.x > 0.0, normal.y > 0.0, normal.z > 0.0);
    ivec3 cube_indices = ivec3(is_positive.x ? kPositiveX : kNegativeX,
                               is_positive.y ? kPositiveY : kNegativeY,
                               is_positive.z ? kPositiveZ : kNegativeZ);
    ambient_cube[cube_indices.x] = is_positive.x ?
                                       vec3(texture(irradiance_volume_px, irradiance_sample_pos.xyz).rgb) :
                                       vec3(texture(irradiance_volume_nx, irradiance_sample_pos.xyz).rgb);
    ambient_cube[cube_indices.y] = is_positive.y ?
                                       vec3(texture(irradiance_volume_py, irradiance_sample_pos.xyz).rgb) :
                                       vec3(texture(irradiance_volume_ny, irradiance_sample_pos.xyz).rgb);
    ambient_cube[cube_indices.z] = is_positive.z ?
                                       vec3(texture(irradiance_volume_pz, irradiance_sample_pos.xyz).rgb) :
                                       vec3(texture(irradiance_volume_nz, irradiance_sample_pos.xyz).rgb);
    return SampleAmbientCube(ambient_cube, normal);
}

void main(void)
{
    vec2 sample_pos = tex_coord * 2.0f - 1.0f;
    uint cube_face = gl_Layer;

    // -1.0f because we use right handed coordinates and this faces the same way as the camera
    vec4 sample_dir = inv_direction_matrices[cube_face] * vec4(sample_pos, -1.0f, 1.0f);
    sample_dir /= sample_dir.w;
    sample_dir = normalize(sample_dir);

    float depth_sample = texture(depth, sample_dir.xyz).r;

    if (depth_sample == 1.0)
    {
        env_map_colour = vec4(SkyLight(sample_dir.xyz, sh_sky_colour, sky_luminance), 1.0);
        ld_term_colour = env_map_colour;
        return;
    }

    vec3 surface_albedo = texture(albedo, sample_dir.xyz).rgb;
    vec3 surface_normal = texture(normal, sample_dir.xyz).rgb * 2.0f - 1.0f;
    float surface_depth = depth_sample * 2.0f - 1.0f;

    vec4 world_pos = inv_vp_matrices[cube_face] * vec4(sample_pos, surface_depth, 1.0f);
    world_pos /= world_pos.w;

    float light_visibility = ShadowTest(world_pos, light_vp_matrix, light_depth);
    float NdotL = max(dot(surface_normal, -sun.dir), 0.0f);

    // Simple lambert diffuse term since specular is not calculated in probe relighting,
    // meaning no normalized terms are needed
    vec3 diffuse = surface_albedo * light_visibility * sun.colour * sun.luminance * NdotL;
    diffuse += surface_albedo * AmbientDiffuse(world_pos, surface_normal);
    diffuse /= kPi;

    env_map_colour = vec4(diffuse, 1.0f);
    ld_term_colour = env_map_colour;
}