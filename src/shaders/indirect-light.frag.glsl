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
in float probe_id;

out vec4 frag_colour;

// Globals
uniform mat4 inv_vp_matrix;
uniform vec2 screen;
uniform float probe_count;
uniform float probe_distance;
uniform sampler2D normal;
uniform sampler2D view_depth;
uniform sampler2D probe_coefficients;

// SH basis pre-computed constants (so we don't do a billion sqrts in the shader code)
// L0m0 = 1 / (2 sqrt(pi))
const float kL0m0 = 0.282095;
// L1m-1 = -( (sqrt(3) y) / (2 sqrt(pi)))
const float kL1m_1 = -0.488603;
// L1m0 = (sqrt(3) z) / (2 sqrt(pi))
const float kL1m0 = 0.488603;
// L1m1 = -(sqrt(3) x) / (2 sqrt(pi))
const float kL1m1 = -0.488603;
// L2m-2 = (sqrt(15) y x) / (2 sqrt(pi))
const float kL2m_2 = 1.092548;
// L2m-1 = -(sqrt(15) y z) / (2 sqrt(pi))
const float kL2m_1 = -1.092548;
// L2m0 = (sqrt(5) (3 z^2 - 1)) / (4 sqrt(pi))
const float kL2m0 = 0.315392;
// L2m1 = -(sqrt(15) x z) / (2 sqrt(pi))
const float kL2m1 = -1.092548;
// L2m2 = (sqrt(15) (x^2 - y^2)) / (4 sqrt(pi))
const float kL2m2 = 0.546274;

const float coef_count = 9;

vec3 SampleSH(float coef_index)
{
	return texture(probe_coefficients, vec2((coef_index + 0.5) / coef_count, (probe_id + 0.5) / probe_count)).rgb;
}

vec3 IrradianceFromSH(vec3 n)
{
	vec3 irradiance = vec3(0);
	irradiance += kL0m0 * SampleSH(0);
	irradiance += kL1m_1 * n.y * SampleSH(1);
	irradiance += kL1m0 * n.z * SampleSH(2);
	irradiance += kL1m1 * n.x * SampleSH(3);
	irradiance += kL2m_2 * n.y * n.x * SampleSH(4);
	irradiance += kL2m_1 * n.y * n.z * SampleSH(5);
	irradiance += kL2m0 * (3.0 * (n.z * n.z) - 1.0) * SampleSH(6);
	irradiance += kL2m1 * n.x * n.z * SampleSH(7);
	irradiance += kL2m2 * (n.x * n.x - n.y * n.y) * SampleSH(8);

	return max(irradiance, vec3(0.0));
}

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

	vec3 light_dir = probe_pos - surface_pos.xyz;
	float distance = length(light_dir);
	if (distance > probe_distance)
	{
		discard;
	}
	light_dir = normalize(light_dir);

	vec3 surface_normal = normalize(texture(normal, tex_coord).rgb * 2.0 - 1.0);
	float light_angle = dot(surface_normal, light_dir);

	if (light_angle < 0.0)
	{
		discard;
	}

	//vec4 dump = texture(probe_coefficients, tex_coord) * surface_pos.x * surface_normal.x * light_dir.x * light_angle * probe_distance * depth * 0.0000001;
	// Quadratic falloff makes a smoother transition between light probes
	float dist_coef = pow((probe_distance - distance) / probe_distance, 2);
	float intensity = dist_coef * light_angle;// + dump;;
	const float gi_gain = 3.0f;
	frag_colour = vec4(IrradianceFromSH(surface_normal) * gi_gain * intensity, intensity);
	//frag_colour += vec4(surface_pos.rgb / 1, 1.0);
	//frag_colour += vec4(((surface_normal + 1) / 2) * 0.2, 1.0);
}