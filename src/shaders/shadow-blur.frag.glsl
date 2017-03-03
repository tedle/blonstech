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

out vec4 frag_colour;

// Globals
uniform sampler2D blur_texture;
uniform int texture_resolution;
uniform int direction;

void main(void)
{
    const int kernel_size = 5;
    int blur_width = kernel_size * direction;
    int blur_height = kernel_size * (1 - direction);

    vec4 blur_frag = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    // Use the GPU to sample halfway between pixels (free blur!)
    // and then increment in steps of 2
    for (int x = -blur_width; x <= blur_width; x += 2)
    {
        for (int y = -blur_height; y <= blur_height; y += 2)
        {
            vec2 vert_step = vec2(x + 0.5f, y + 0.5f);
            vert_step /= texture_resolution;
            blur_frag += vec4(texture(blur_texture, tex_coord + vert_step).rg, 0.0f, 1.0f);
        }
    }
    frag_colour = blur_frag / ((blur_width + 1) * (blur_height + 1));
}
