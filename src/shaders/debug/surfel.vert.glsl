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

// Ins n outs
in vec3 input_pos;

out vec3 radiance;

// Globals
uniform mat4 world_matrix;
uniform mat4 vp_matrix;

layout(std430) buffer surfel_buffer
{
    Surfel surfels[];
};

void main(void)
{
    Surfel s = surfels[gl_InstanceID];

    // Invert Z to build for RH coordinates
    vec3 z_basis = normalize(vec3(-s.normal[0], -s.normal[1], -s.normal[2]));
    vec3 x_basis = normalize(cross(vec3(0.0, 1.0, 0.0), z_basis));
    vec3 y_basis = cross(x_basis, z_basis);
    mat4 rotation_matrix = mat4(
        x_basis.x, x_basis.y, x_basis.z, 0,
        y_basis.x, y_basis.y, y_basis.z, 0,
        z_basis.x, z_basis.y, z_basis.z, 0,
        0,         0,         0,         1
    );
    // Rotate quad to face normal -> resize to surfel width -> offset by surfel pos
    gl_Position = rotation_matrix * world_matrix * vec4(input_pos, 1.0) + vec4(s.pos[0], s.pos[1], s.pos[2], 0.0);
    gl_Position = vp_matrix * gl_Position;

    radiance = vec3(s.radiance[0], s.radiance[1], s.radiance[2]);
}
