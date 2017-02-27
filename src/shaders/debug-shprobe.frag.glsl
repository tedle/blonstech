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
#include <shaders/colour.lib.glsl>
#include <shaders/math.lib.glsl>
#include <shaders/types.lib.glsl>

// Ins n outs
in vec2 tex_coord;
in mat3 norm;

out vec4 frag_colour;

// Globals
uniform sampler2D probe_env_maps_albedo;
uniform sampler2D probe_env_maps_normal;
uniform mat4 env_proj_matrix;
uniform int env_tex_size;
uniform int probe_id;
uniform float exposure;
uniform int debug_mode;

layout(std430) buffer probe_buffer
{
    Probe probes[];
};

// Re-orients surface normal to fit within cube face frustum (-Z normal, RH coordinates)
// i.e.,  inv_rotation_matrices[1/*+X*/] * vec3(1,0,0) = vec3(0,0,-1)
const mat3 inv_rotation_matrices[6] = {
    mat3( // -Z
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    ),
    mat3( // +X
        0, 0,-1,
        0, 1, 0,
        1, 0, 0
    ),
    mat3( // +Z
       -1, 0, 0,
        0, 1, 0,
        0, 0,-1
    ),
    mat3( // -X
        0, 0, 1,
        0, 1, 0,
       -1, 0, 0
    ),
    mat3( // +Y
        1, 0, 0,
        0, 0,-1,
        0, 1, 0
    ),
    mat3( // -Y
        1, 0, 0,
        0, 0, 1,
        0,-1, 0
    )
};

void main(void)
{
    // equivilent of transpose(norm)[2].xyz;
    vec3 surface_normal = normalize(vec3(norm[0].z, norm[1].z, norm[2].z));
    vec3 abs_normal = abs(surface_normal);
    // Since branching in shaders is bad, welcome to a really ugly ternary operator
    int face_index =
        // if
        (abs_normal.x > abs_normal.y && abs_normal.x > abs_normal.z ?
            (surface_normal.x > 0.0f ? 1 : 3) :
        // else if
        (abs_normal.y > abs_normal.z ?
            (surface_normal.y > 0.0f ? 4 : 5) :
        // else
            (surface_normal.z > 0.0f ? 2 : 0)));
    vec3 local_surface_normal = inv_rotation_matrices[face_index] * surface_normal;

    vec4 texel_pos = env_proj_matrix * vec4(local_surface_normal, 1.0);
    texel_pos /= texel_pos.w;
    texel_pos.xy = (texel_pos.xy + 1.0f) / 2.0f;

    texel_pos.xy *= env_tex_size;
    // Offset in env map texture atlas
    texel_pos.xy += vec2(face_index * env_tex_size, probe_id * env_tex_size);
    // Normalize
    texel_pos.x /= env_tex_size * 6;
    texel_pos.y /= env_tex_size * probes.length();

    // Calculate ambient cube data
    vec3 ambient_cube[6] = vec3[6](
        vec3(probes[probe_id].cube_coeffs[kPositiveX][0], probes[probe_id].cube_coeffs[kPositiveX][1], probes[probe_id].cube_coeffs[kPositiveX][2]),
        vec3(probes[probe_id].cube_coeffs[kNegativeX][0], probes[probe_id].cube_coeffs[kNegativeX][1], probes[probe_id].cube_coeffs[kNegativeX][2]),
        vec3(probes[probe_id].cube_coeffs[kPositiveY][0], probes[probe_id].cube_coeffs[kPositiveY][1], probes[probe_id].cube_coeffs[kPositiveY][2]),
        vec3(probes[probe_id].cube_coeffs[kNegativeY][0], probes[probe_id].cube_coeffs[kNegativeY][1], probes[probe_id].cube_coeffs[kNegativeY][2]),
        vec3(probes[probe_id].cube_coeffs[kPositiveZ][0], probes[probe_id].cube_coeffs[kPositiveZ][1], probes[probe_id].cube_coeffs[kPositiveZ][2]),
        vec3(probes[probe_id].cube_coeffs[kNegativeZ][0], probes[probe_id].cube_coeffs[kNegativeZ][1], probes[probe_id].cube_coeffs[kNegativeZ][2])
    );
    vec3 ambient_light = SampleAmbientCube(ambient_cube, surface_normal);
    // Ambient cubes store irradiance data, view as exit irradiance with pi division
    ambient_light /= kPi;
    ambient_light = FilmicTonemap(ambient_light * exposure);

    // Calculate direct sky vis data
    float sh_normal[9];
    SHProjectDirection3(surface_normal, sh_normal);
    float sky_vis = SHDot3(sh_normal, probes[probe_id].sh_coeffs);
    // Calculate sky vis as viewed on a diffuse material
    SHProjectCosineLobe3(surface_normal, sh_normal);
    // Divide by pi to turn irradiance into exit radiance
    float sky_vis_diffuse = SHDot3(sh_normal, probes[probe_id].sh_coeffs) / kPi;

    // Encode probe id into 3 colour channels
    vec3 id_colour;
    id_colour.r = float( probe_id & 0xFF           ) / 255.0f;
    id_colour.g = float((probe_id & 0xFF00)   >>  8) / 255.0f;
    id_colour.b = float((probe_id & 0xFF0000) >> 16) / 255.0f;

    frag_colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    frag_colour.rgb += texture(probe_env_maps_albedo, texel_pos.xy).rgb * (debug_mode == 1 ? 1.0f : 0.0f);
    frag_colour.rgb += texture(probe_env_maps_normal, texel_pos.xy).rgb * (debug_mode == 2 ? 1.0f : 0.0f);
    frag_colour.rgb += (surface_normal + 1.0f) / 2.0f                   * (debug_mode == 3 ? 1.0f : 0.0f);
    frag_colour.rgb += ambient_light                                    * (debug_mode == 4 ? 1.0f : 0.0f);
    frag_colour.rgb += sky_vis                                          * (debug_mode == 5 ? 1.0f : 0.0f);
    frag_colour.rgb += sky_vis_diffuse                                  * (debug_mode == 6 ? 1.0f : 0.0f);
    frag_colour.rgb += id_colour                                        * (debug_mode == 7 ? 1.0f : 0.0f);
}
