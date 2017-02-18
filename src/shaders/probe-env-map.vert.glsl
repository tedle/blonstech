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
in vec3 input_pos;
in vec2 input_uv;
in vec3 input_norm;
in vec3 input_tan;
in vec3 input_bitan;

out vec2 tex_coord;
out vec2 scissor_coord;
out mat3 norm;

// Globals
uniform mat4 m_matrix;
uniform mat4 normal_matrix;
uniform int scissor_w;
uniform int scissor_h;
uniform int map_width;
uniform int map_height;

struct PerFaceData
{
    float[4][4] vp_matrix;
    int scissor_x;
    int scissor_y;
};

layout(std430) buffer per_face_data_buffer
{
    PerFaceData per_face_data[];
};

void main(void)
{
    // Fetch unique face data
    PerFaceData face_data = per_face_data[gl_InstanceID];
    // Reconstruct mat4 from float array. We do this to bypass the 16-byte struct alignment caused by using mat4s
    // This allows for the C++ client to abstract away any padding concerns
    mat4 vp_matrix = mat4(
        face_data.vp_matrix[0][0], face_data.vp_matrix[0][1], face_data.vp_matrix[0][2], face_data.vp_matrix[0][3],
        face_data.vp_matrix[1][0], face_data.vp_matrix[1][1], face_data.vp_matrix[1][2], face_data.vp_matrix[1][3],
        face_data.vp_matrix[2][0], face_data.vp_matrix[2][1], face_data.vp_matrix[2][2], face_data.vp_matrix[2][3],
        face_data.vp_matrix[3][0], face_data.vp_matrix[3][1], face_data.vp_matrix[3][2], face_data.vp_matrix[3][3]
    );
    scissor_coord = vec2(float(face_data.scissor_x), float(face_data.scissor_y));
    // Calculate vertex position in global clip space
    vec4 pos = vp_matrix * m_matrix * vec4(input_pos, 1.0);
    // Calculate viewport coordinates in global clip space
    vec2 map_size = vec2(float(map_width), float(map_height));
    vec2 box_pos = scissor_coord / map_size;
    vec2 box_size = vec2(float(scissor_w), float(scissor_h)) / map_size;
    // Calculate vertex position in screen space
    vec2 screen_pos = (pos.xy / pos.w + 1.0) / 2.0;
    // Calculate vertex position in viewport clip space
    vec2 clip_pos = (screen_pos * box_size + box_pos) * 2.0 - 1.0;
    // Construct gl_Position from constrained clip space coordinates
    gl_Position = vec4(clip_pos.xy * pos.w, pos.z, pos.w);

    tex_coord = input_uv;
    norm = transpose(mat3(normalize((normal_matrix * vec4(input_tan, 1.0f)).xyz),
                          normalize((normal_matrix * vec4(input_bitan, 1.0f)).xyz),
                          normalize((normal_matrix * vec4(input_norm, 1.0f)).xyz)));
}
