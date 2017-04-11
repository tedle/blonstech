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
#include <shaders/lib/colour.lib.glsl>
#include <shaders/lib/math.lib.glsl>
#include <shaders/lib/types.lib.glsl>
#include <shaders/lib/probes.lib.glsl>

// Ins n outs
in vec2 tex_coord;
in mat3 norm;

out vec4 frag_colour;

// Globals
uniform int probe_id;
uniform float probe_weight;
uniform float exposure;
uniform int debug_mode;
uniform vec3 camera_position;

void main(void)
{
    // equivilent of transpose(norm)[2].xyz;
    vec3 surface_normal = normalize(vec3(norm[0].z, norm[1].z, norm[2].z));

    // Calculate ambient cube data
    Probe probe = FindProbe(probe_id);
    vec3 ambient_cube[6] = vec3[6](
        vec3(probe.cube_coeffs[kPositiveX][0], probe.cube_coeffs[kPositiveX][1], probe.cube_coeffs[kPositiveX][2]),
        vec3(probe.cube_coeffs[kNegativeX][0], probe.cube_coeffs[kNegativeX][1], probe.cube_coeffs[kNegativeX][2]),
        vec3(probe.cube_coeffs[kPositiveY][0], probe.cube_coeffs[kPositiveY][1], probe.cube_coeffs[kPositiveY][2]),
        vec3(probe.cube_coeffs[kNegativeY][0], probe.cube_coeffs[kNegativeY][1], probe.cube_coeffs[kNegativeY][2]),
        vec3(probe.cube_coeffs[kPositiveZ][0], probe.cube_coeffs[kPositiveZ][1], probe.cube_coeffs[kPositiveZ][2]),
        vec3(probe.cube_coeffs[kNegativeZ][0], probe.cube_coeffs[kNegativeZ][1], probe.cube_coeffs[kNegativeZ][2])
    );
    vec3 ambient_light = SampleAmbientCube(ambient_cube, surface_normal);
    // Ambient cubes store irradiance data, view as exit irradiance with pi division
    ambient_light /= kPi;
    ambient_light = GammaEncode(FilmicTonemap(ambient_light * exposure));

    // Calculate direct sky vis data
    float sh_normal[9];
    SHProjectDirection3(surface_normal, sh_normal);
    float sky_vis = SHDot3(sh_normal, probe.sh_coeffs);
    // Calculate sky vis as viewed on a diffuse material
    SHProjectCosineLobe3(surface_normal, sh_normal);
    // Divide by pi to turn irradiance into exit radiance
    float sky_vis_diffuse = SHDot3(sh_normal, probe.sh_coeffs) / kPi;

    // Encode probe id into 3 colour channels
    vec3 id_colour;
    id_colour.r = float( probe_id & 0xFF           ) / 255.0f;
    id_colour.g = float((probe_id & 0xFF00)   >>  8) / 255.0f;
    id_colour.b = float((probe_id & 0xFF0000) >> 16) / 255.0f;

    ProbeWeight[4] weights = FindProbeWeights(camera_position);
    float probe_weight_shader = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        if (weights[i].id == probe_id)
        {
            probe_weight_shader = weights[i].weight;
        }
    }

    frag_colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    frag_colour.rgb += ambient_light                  * (debug_mode == 1 ? 1.0f : 0.0f);
    frag_colour.rgb += sky_vis                        * (debug_mode == 2 ? 1.0f : 0.0f);
    frag_colour.rgb += sky_vis_diffuse                * (debug_mode == 3 ? 1.0f : 0.0f);
    frag_colour.r   += probe_weight                   * (debug_mode == 4 ? 1.0f : 0.0f);
    frag_colour.b   += probe_weight_shader            * (debug_mode == 4 ? 1.0f : 0.0f);
    frag_colour.rgb += (surface_normal + 1.0f) / 2.0f * (debug_mode == 5 ? 1.0f : 0.0f);
    frag_colour.rgb += id_colour                      * (debug_mode == 6 ? 1.0f : 0.0f);
}
