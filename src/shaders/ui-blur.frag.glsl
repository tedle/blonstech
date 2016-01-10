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
uniform sampler2D composite;
uniform sampler2D ui;
uniform float horizontal;
uniform float screen_length;

void main(void)
{
    vec2 dir = vec2(horizontal, 1.0 - horizontal);
    float step = (1.0 / screen_length);
    vec3 blur_colour = vec3(0);
    float shadow = 0;
    // Glass blur
    blur_colour.rgb += texture(composite, tex_coord - step * 5.230769230769231 * dir).rgb * 0.07027027027027027;
    blur_colour.rgb += texture(composite, tex_coord - step * 2.3846153846153848 * dir).rgb * 0.3162162162162162;
    blur_colour.rgb += texture(composite, tex_coord).rgb * 0.22702702702702704;
    blur_colour.rgb += texture(composite, tex_coord + step * 2.3846153846153848 * dir).rgb * 0.3162162162162162;
    blur_colour.rgb += texture(composite, tex_coord + step * 5.230769230769231 * dir).rgb * 0.07027027027027027;
    // Drop shadow
    shadow += texture(ui, tex_coord - step * 5.230769230769231 * dir).a * 0.07027027027027027;
    shadow += texture(ui, tex_coord - step * 2.3846153846153848 * dir).a * 0.3162162162162162;
    shadow += texture(ui, tex_coord).a * 0.22702702702702704;
    shadow += texture(ui, tex_coord + step * 2.3846153846153848 * dir).a * 0.3162162162162162;
    shadow += texture(ui, tex_coord + step * 5.230769230769231 * dir).a * 0.07027027027027027;
    frag_colour = vec4(blur_colour, shadow);
}