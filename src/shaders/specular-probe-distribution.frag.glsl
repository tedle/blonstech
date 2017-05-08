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
#include <shaders/lib/math.lib.glsl>
#include <shaders/lib/pbr.lib.glsl>

// Ins n outs
in vec2 tex_coord;

out vec4 ld_term_colour;

// Globals
uniform mat4 inv_direction_matrices[6];
uniform samplerCube environment_map;
uniform int mip_level;
uniform int max_mip_level;
uniform int base_texture_size;

vec3 IntegrateLDTerm(vec3 normal, float roughness, int base_texture_size)
{
    // Needed in a hot path, might as well pre-calculate
    const float env_solid_angle = 4.0f * kPi / (float(base_texture_size) * float(base_texture_size) * 6.0f);
    vec3 view = normal;
    vec3 ld_term = vec3(0.0f);
    float ld_weight = 1e-5f;
    // Cap mip level access during pre-filtered sampling at the 8x8 texture
    const float max_mip_level = log2(float(base_texture_size)) - 3.0f; // - log2(8)
    const int kSampleCount = 32;
    for (int i = 0; i < kSampleCount; ++i)
    {
        vec2 random_sample = LowDiscrepancySample2D(i, kSampleCount);
        vec3 halfway = ImportanceSampleGGX(random_sample, normal, roughness);
        vec3 light = reflect(-view, halfway);
        float NdotL = clamp(dot(normal, light), 0.0f, 1.0f);
        if (NdotL > 0.0f)
        {
            float NdotH = dot(normal, halfway);
            // Normally needs ndf * NdotH / (4.0 * LdotH), but the terms cancel out as N == V
            float pdf = SpecularNormalDistributionFunction(NdotH, roughness) / 4.0f;
            float pdf_solid_angle = 1.0f / (pdf * float(kSampleCount));
            // Sample the environment map's mip level based on the probability size of
            // a sample against the solid angle of an environment map texel. This vastly
            // improves convergence (like really a lot)
            // See: https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html
            float lod = clamp(0.5f * log2(pdf_solid_angle / env_solid_angle), 0.0f, max_mip_level);
            ld_term += textureLod(environment_map, light, lod).rgb * NdotL;
            ld_weight += NdotL;
        }
    }
    return ld_term / ld_weight;
}

void main(void)
{
    vec2 sample_pos = tex_coord * 2.0f - 1.0f;
    uint cube_face = gl_Layer;

    // -1.0f because we use right handed coordinates and this faces the same way as the camera
    vec4 sample_dir = inv_direction_matrices[cube_face] * vec4(sample_pos, -1.0f, 1.0f);
    sample_dir /= sample_dir.w;

    vec3 normal = normalize(sample_dir.xyz);
    float roughness = MipLevelToRoughness(mip_level, max_mip_level);
    ld_term_colour = vec4(IntegrateLDTerm(normal, roughness, base_texture_size), 1.0f);
}