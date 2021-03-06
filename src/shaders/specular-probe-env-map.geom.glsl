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

// Layout information
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;
// 1 instance for each cubemap face
layout(invocations = 6) in;

// Ins n outs
in vec2 vertex_tex_coord[];
in mat3 vertex_norm[];

out vec2 tex_coord;
out mat3 norm;

// Globals
uniform mat4 vp_matrices[6];

void main(void)
{
    gl_Layer = gl_InvocationID;
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = vp_matrices[gl_Layer] * gl_in[i].gl_Position;
        tex_coord = vertex_tex_coord[i];
        norm = vertex_norm[i];
        EmitVertex();
    }
    EndPrimitive();
}
