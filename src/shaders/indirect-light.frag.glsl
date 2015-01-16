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
in vec3 probe_pos;

out vec4 frag_colour;

// Globals
uniform mat4 inv_vp_matrix;
uniform vec2 screen;
uniform float probe_distance;
uniform sampler2D normal;
uniform sampler2D view_depth;
uniform sampler2D probe_coefficients;

// TODO: This is really slow, find some way to clip probes more efficiently...
void main(void)
{
	vec2 tex_coord = vec2(gl_FragCoord.xy) / screen;

	float depth = texture(view_depth, tex_coord).r;

	// World coordinates of the pixel we're rendering over
	vec4 surface_pos = vec4(tex_coord.x,
							tex_coord.y,
							depth,
							1.0);
	surface_pos = inv_vp_matrix * (surface_pos * 2.0 - 1.0);
	surface_pos /= surface_pos.w;

	if (length(probe_pos - surface_pos.xyz) > 5.0)
	{
		discard;
	}

	vec3 surface_normal = normalize(texture(normal, tex_coord).rgb * 2.0 - 1.0);
	vec3 light = probe_pos - surface_pos.xyz;
	float light_angle = clamp(dot(surface_normal, normalize(light)), 0.0, 1.0);

	if (light_angle < 0.0)
	{
		discard;
	}

	//frag_colour = vec4(vec3(light_angle), 1.0f);

	vec4 dump = texture(probe_coefficients, tex_coord) * light_angle * probe_distance * depth * 0.0000001;
	frag_colour += dump;
	frag_colour += vec4(surface_pos.rgb / 1, 1.0);
	frag_colour += vec4(((surface_normal + 1) / 2) * 0.2, 1.0);
}