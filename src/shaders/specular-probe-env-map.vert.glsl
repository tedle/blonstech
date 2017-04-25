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

// Ins n outs
in vec3 input_pos;
in vec2 input_uv;
in vec3 input_norm;
in vec3 input_tan;
in vec3 input_bitan;

out vec2 vertex_tex_coord;
out mat3 vertex_norm;

// Globals
uniform mat4 model_matrix;
uniform mat4 normal_matrix;

void main(void)
{
    gl_Position = model_matrix * vec4(input_pos, 1.0);

    vertex_tex_coord = input_uv;
    vertex_norm = transpose(mat3(normalize((normal_matrix * vec4(input_tan, 1.0f)).xyz),
                                 normalize((normal_matrix * vec4(input_bitan, 1.0f)).xyz),
                                 normalize((normal_matrix * vec4(input_norm, 1.0f)).xyz)));
}
