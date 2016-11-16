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

in float depth;

out vec4 frag_colour;

void main(void)
{
    // Doesn't need to be [0,1], but makes debugging via
    // looking at textures easier... but makes for lost
    // precision as well
    float out_depth = (depth + 1.0) / 2.0;
    frag_colour.r = out_depth;
    // Used to compute the mean when texel discrepencies pop up
    frag_colour.g = out_depth * out_depth;
    frag_colour.a = 1.0f;

    // Should help reduce biasing on sharp angles...?
    vec2 d = vec2(dFdx(out_depth), dFdy(out_depth));
    frag_colour.g += 0.25 * (d.x * d.x + d.y * d.y);
}