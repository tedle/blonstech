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
//include <something>

// Ins n outs
in vec2 tex_coord;

out vec4 ld_term_colour;

// Globals
uniform mat4 inv_direction_matrices[6];
uniform samplerCube environment_map;
uniform float roughness;

void main(void)
{
    vec2 sample_pos = tex_coord * 2.0f - 1.0f;
    uint cube_face = gl_Layer;

    // -1.0f because we use right handed coordinates and this faces the same way as the camera
    vec4 sample_dir = inv_direction_matrices[cube_face] * vec4(sample_pos, -1.0f, 1.0f);
    sample_dir /= sample_dir.w;
    sample_dir = normalize(sample_dir);

    vec3 ld_term = textureLod(environment_map, sample_dir.xyz, 0.0f).rgb;
    ld_term += roughness;

    ld_term_colour = vec4(ld_term, 1.0f);
}