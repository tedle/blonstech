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

#version 400

// Ins n outs
in vec3 probe_pos;
in float probe_id;

out vec4 frag_colour;

// Globals
uniform float probe_count;
uniform float probe_distance;
uniform float lightmap_resolution;
uniform sampler2D pos_lookup;
uniform sampler2D norm_lookup;
uniform sampler2D probe_coefficients;

const float C0 = 1.0;
const float C1 = 0.3;
const float C2 = 0.5;
const float C3 = 0.4;
const float C4 = 0.4;

// SH basis pre-computed constants (so we don't do a billion sqrts in the shader code)
// I really don't understand spherical harmonics and had to randomly change stuff until it looked okay
// Sorry. I'm leaving the more accurate stuff here in case I ever figure this stuff out
// L0m0 = 1 / (2 sqrt(pi))
float kL0m0 = C0;
// L1m-1 = -( (sqrt(3) y) / (2 sqrt(pi)))
float kL1m_1 = C1;// * -1;
// L1m0 = (sqrt(3) z) / (2 sqrt(pi))
float kL1m0 = C1;
// L1m1 = -(sqrt(3) x) / (2 sqrt(pi))
float kL1m1 = C1;// * -1;
// L2m-2 = (sqrt(15) y x) / (2 sqrt(pi))
float kL2m_2 = C2;
// L2m-1 = -(sqrt(15) y z) / (2 sqrt(pi))
float kL2m_1 = C2;// * -1;
// L2m0 = (sqrt(5) (3 z^2 - 1)) / (4 sqrt(pi))
float kL2m0 = C3;
// L2m1 = -(sqrt(15) x z) / (2 sqrt(pi))
float kL2m1 = C2;// * -1;
// L2m2 = (sqrt(15) (x^2 - y^2)) / (4 sqrt(pi))
float kL2m2 = C4;

const float coef_count = 9;

vec3 SampleSH(float coef_index)
{
    return texture(probe_coefficients, vec2((coef_index + 0.5) / coef_count, (probe_id + 0.5) / probe_count)).rgb;
}

vec3 IrradianceFromSH(vec3 n)
{
    vec3 irradiance = vec3(0);
    irradiance += kL0m0 * SampleSH(0);
    irradiance += kL1m_1 * n.y * SampleSH(1);
    irradiance += kL1m0 * n.z * SampleSH(2);
    irradiance += kL1m1 * n.x * SampleSH(3);
    irradiance += kL2m_2 * n.y * n.x * SampleSH(4);
    irradiance += kL2m_1 * n.y * n.z * SampleSH(5);
    irradiance += kL2m0 * (3.0 * (n.z * n.z) - 1.0) * SampleSH(6);
    irradiance += kL2m1 * n.x * n.z * SampleSH(7);
    irradiance += kL2m2 * (n.x * n.x - n.y * n.y) * SampleSH(8);

    return max(irradiance, vec3(0.0));
}

void main(void)
{
    vec2 tex_coord = gl_FragCoord.xy / lightmap_resolution;

    vec3 surface_pos = texture(pos_lookup, tex_coord).rgb;

    vec3 light_dir = probe_pos - surface_pos.xyz;
    float distance = length(light_dir);
    if (distance > probe_distance)
    {
        discard;
    }
    light_dir = normalize(light_dir);

    vec3 surface_normal = texture(norm_lookup, tex_coord).rgb * 2 - 1;
    float light_angle = dot(surface_normal, light_dir);

    if (light_angle < 0.0)
    {
        discard;
    }

    // Quadratic falloff makes a smoother transition between light probes
    float dist_coef = pow((probe_distance - distance) / probe_distance, 1.3);
    float intensity = dist_coef * light_angle;
    const float correction_factor = 0.1f;
    frag_colour = vec4(IrradianceFromSH(surface_normal) * correction_factor * intensity, intensity);
}