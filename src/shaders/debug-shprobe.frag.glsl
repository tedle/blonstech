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

#version 430

// Ins n outs
in vec2 tex_coord;
in mat3 norm;

out vec4 frag_colour;

// Globals
uniform sampler2D probe_env_maps;
uniform mat4 env_proj_matrix;
uniform int env_tex_size;
uniform int probe_count;
uniform int probe_id;

// Re-orients surface normal to fit within cube face frustum (-Z normal, RH coordinates)
// i.e., vec3(1,0,0) * inv_rotation_matrices[1/*+X*/] = vec3(0,0,-1)
const mat3 inv_rotation_matrices[6] = {
    mat3( // -Z
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    ),
    mat3( // +X
        0, 0, 1,
        0, 1, 0,
       -1, 0, 0
    ),
    mat3( // +Z
       -1, 0, 0,
        0, 1, 0,
        0, 0,-1
    ),
    mat3( // -X
        0, 0,-1,
        0, 1, 0,
        1, 0, 0
    ),
    mat3( // +Y
        1, 0, 0,
        0, 0, 1,
        0,-1, 0
    ),
    mat3( // -Y
        1, 0, 0,
        0, 0,-1,
        0, 1, 0
    )
};

void main(void)
{
    // equivilent of transpose(norm)[2].xyz;
    vec3 surface_normal = vec3(norm[0].z, norm[1].z, norm[2].z);
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
    surface_normal *= inv_rotation_matrices[face_index];

    vec4 texel_pos = env_proj_matrix * vec4(surface_normal, 1.0);
    texel_pos /= texel_pos.w;
    texel_pos.xy = (texel_pos.xy + 1.0f) / 2.0f;

    texel_pos.xy *= env_tex_size;
    // Offset in env map texture atlas
    texel_pos.xy += vec2(face_index * env_tex_size, probe_id * env_tex_size);
    // Normalize
    texel_pos.x /= env_tex_size * 6;
    texel_pos.y /= env_tex_size * probe_count;
    frag_colour = vec4(texture(probe_env_maps, texel_pos.xy).rgb, 1.0);
}
