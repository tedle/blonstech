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

// Workgroup size
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// Globals
layout(rgba16, binding = 0) writeonly uniform image2D brdf_lut;

vec4 IntegrateSplitGGXTerm(float NdotV, float roughness)
{
    vec3 normal = vec3(0.0f, 0.0f, 1.0f);
    vec3 view = vec3(0.0f, sqrt(1.0f - NdotV * NdotV), NdotV);
    vec2 split_dfg = vec2(0.0f);
    vec2 split_diffuse = vec2(0.0f);
    const int kSampleCount = 1024;
    for (int i = 0; i < kSampleCount; i++)
    {
        vec2 random_sample = LowDiscrepancySample2D(i, kSampleCount);
        // Split specular term
        vec3 halfway = ImportanceSampleGGX(random_sample, normal, roughness);
        vec3 light = reflect(-view, halfway);
        float NdotL = clamp(dot(normal, light), 0.0f, 1.0f);
        if (NdotL > 0.0f)
        {
            float NdotH = dot(normal, halfway);
            float LdotH = dot(light, halfway);
            split_dfg += SpecularTermGGXSplit(roughness, NdotH, NdotL, NdotV, LdotH);
        }
        // Split diffuse term
        light = ImportanceSampleCosineLobe(random_sample, normal);
        halfway = normalize(view + light);
        NdotL = clamp(dot(normal, light), 0.0f, 1.0f);
        if (NdotL > 0.0f)
        {
            float NdotH = dot(normal, halfway);
            float LdotH = dot(light, halfway);
            float LdotV = dot(light, view);
            split_diffuse += DiffuseTermGGXSplit(NdotV, NdotL, LdotH, LdotV, roughness);
        }
    }
    return vec4(split_dfg, split_diffuse) / kSampleCount;
}

void main(void)
{
    // Figure out the UV-space coordinates of this invocation
    vec2 texels = vec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);
    vec2 sample_pos = (vec2(gl_GlobalInvocationID.xy) + vec2(0.5f)) / texels;
    float NdotV = sample_pos.x;
    float roughness = sample_pos.y;

    imageStore(brdf_lut, ivec2(gl_GlobalInvocationID.xy), IntegrateSplitGGXTerm(NdotV, roughness));
}