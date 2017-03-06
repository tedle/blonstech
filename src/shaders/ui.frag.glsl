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
in vec2 tex_coord;
flat in vec4 text_colour;
flat in int is_text;
flat in vec4 crop;
flat in int feather;
flat in int texture_id;
layout(origin_upper_left) in vec4 gl_FragCoord;

out vec4 frag_colour;

// Globals
uniform sampler2D skin[5];

void main(void)
{
    if (is_text == 1)
    {
        float alpha = texture(skin[texture_id], tex_coord).r;
        frag_colour = vec4(text_colour.rgb, text_colour.a * alpha);
    }
    else
    {
        frag_colour = texture(skin[texture_id], tex_coord);
    }
    // Cropping!
    if (crop.z != 0)
    {
        if (gl_FragCoord.x < crop.x + feather)
        {
            float alpha = 0;
            if (feather > 0)
            {
                alpha = max(0, (gl_FragCoord.x - crop.x) / feather);
            }
            frag_colour.a *= alpha;
        }
        else if (gl_FragCoord.x > crop.x + crop.z - feather)
        {
            float alpha = 0;
            if (feather > 0)
            {
                alpha = max(0, (crop.x + crop.z - gl_FragCoord.x) / feather);
            }
            frag_colour.a *= alpha;
        }
    }
    if (crop.w != 0)
    {
        if (gl_FragCoord.y < crop.y + feather)
        {
            float alpha = 0;
            if (feather > 0)
            {
                alpha = max(0, (gl_FragCoord.y - crop.y) / feather);
            }
            frag_colour.a *= alpha;
        }
        else if (gl_FragCoord.y > crop.y + crop.w - feather)
        {
            float alpha = 0;
            if (feather > 0)
            {
                alpha = max(0, (crop.y + crop.w - gl_FragCoord.y) / feather);
            }
            frag_colour.a *= alpha;
        }
    }
}
