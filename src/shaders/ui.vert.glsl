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
in vec2 input_pos;
in vec2 input_uv;

out vec2 tex_coord;
flat out vec4 text_colour;
flat out int is_text;
flat out vec4 crop;
flat out int feather;
flat out int texture_id;

// Globals
uniform mat4 proj_matrix;

layout(std430) buffer drawcall_buffer
{
    UIDrawCallInputs drawcalls[];
};

void main(void)
{
    UIDrawCallInputs drawcall = drawcalls[gl_InstanceID];
    text_colour = vec4(drawcall.colour[0], drawcall.colour[1], drawcall.colour[2], drawcall.colour[3]);
    is_text = drawcall.is_text;
    crop = vec4(drawcall.crop[0], drawcall.crop[1], drawcall.crop[2], drawcall.crop[3]);
    feather = drawcall.crop_feather;
    texture_id = drawcall.texture_id;

    // Convert to [0,1]
    vec2 pos = (input_pos + 1.0) / 2.0;
    pos.y = 1.0 - pos.y;
    // Translate to draw call coords
    pos *= vec2(drawcall.pos[2], drawcall.pos[3]); // .w,h
    pos += vec2(drawcall.pos[0], drawcall.pos[1]); // .x,y
    gl_Position = proj_matrix * vec4(pos, 0.0, 1.0);

    tex_coord = input_uv;
    tex_coord.y = 1.0 - tex_coord.y;
    tex_coord *= vec2(drawcall.uv[2], drawcall.uv[3]); // .w,h
    tex_coord += vec2(drawcall.uv[0], drawcall.uv[1]); // .x,y
}
