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
in mat3 norm;

out vec4 frag_colour;
out vec4 norm_colour;
out vec4 debug_colour;

// Globals
uniform sampler2D albedo;
uniform sampler2D normal;

void main(void)
{
	// Albedo
	frag_colour = vec4(pow(texture(albedo, tex_coord).rgb, vec3(2.2)), 1.0);

	// Normal
	vec3 norm_map = texture(normal, tex_coord).rgb * 2 - 1;
	norm_colour = vec4((norm_map * norm + 1) / 2, 1.0);

	// Debug out (UV coords)
	debug_colour = vec4(tex_coord.xy, 0.0, 1.0);
}