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

#version 400

// Ins n outs
in vec2 tex_coord;

out vec4 frag_colour;

// Globals
uniform sampler2D blurred_composite;
uniform sampler2D ui;

void main(void)
{
    vec4 blurred_composite_frag = texture(blurred_composite, tex_coord);
    vec4 ui_frag = texture(ui, tex_coord);
    // Pseduo stencil test for blur effect
    if (ui_frag.a > 0)
    {
        frag_colour.a = 1.0;
        frag_colour.rgb = mix(blurred_composite_frag.rgb, ui_frag.rgb, ui_frag.a);
    }
    // Apply drop shadow. We should theoretically do this in both paths,
    // however this requires weakening the shadows effect to make it look nice
    // behind windows, causing the actual surrounding drop shadow to be barely visible.
    // Clamping also results in ugly problems
    else
    {
        frag_colour = vec4(0.0, 0.0, 0.0, blurred_composite_frag.a);
    }
}