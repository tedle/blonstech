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
uniform sampler2D probe_albedo;
uniform sampler2D probe_texmap;
uniform sampler2D lightmap;
uniform vec3 sky_colour;

void main(void)
{
	vec3 albedo = texture(probe_albedo, tex_coord).rgb;
	vec3 light_coord = texture(probe_texmap, tex_coord).rgb;

	// Should only ever be 1 or 0 but doesnt hurt to be safe
	if (light_coord.b > 0.5)
	{
		frag_colour = vec4(pow(sky_colour, vec3(2.2)), 1.0);
		return;
	}
	vec4 light_full = texture(lightmap, light_coord.xy);
	vec3 light_colour = light_full.rgb / light_full.a;

	vec3 diffuse = albedo * light_colour;

    frag_colour = vec4(diffuse, 1.0);
}