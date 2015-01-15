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
in vec3 pos;

out vec4 frag_colour;

// Globals
uniform sampler2D probe_irradiance;
uniform int probe_count;
uniform int probe_map_size;

// SH basis pre-computed constants (so we don't do a billion sqrts in the shader code)
// L0m0 = 1 / (2 sqrt(pi))
const float kL0m0 = 0.282095;
// L1m-1 = -( (sqrt(3) y) / (2 sqrt(pi)))
const float kL1m_1 = -0.488603;
// L1m0 = (sqrt(3) z) / (2 sqrt(pi))
const float kL1m0 = kL1m_1 * -1;
// L1m1 = -(sqrt(3) x) / (2 sqrt(pi))
const float kL1m1 = kL1m_1;
// L2m-2 = (sqrt(15) y x) / (2 sqrt(pi))
const float kL2m_2 = 1.092548;
// L2m-1 = -(sqrt(15) y z) / (2 sqrt(pi))
const float kL2m_1 = kL2m_2 * -1;
// L2m0 = (sqrt(5) (3 z^2 - 1)) / (4 sqrt(pi))
const float kL2m0 = 0.315392;
// L2m1 = -(sqrt(15) x z) / (2 sqrt(pi))
const float kL2m1 = kL2m_1;
// L2m2 = (sqrt(15) (x^2 - y^2)) / (4 sqrt(pi))
const float kL2m2 = 0.546274;

// Used for getting the normal of a cubemap sample point
// These are row major, so we transpose the standard rot matrices here
const mat3 kRotationFront = mat3
(
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
);
const mat3 kRotationRear = mat3
(
	-1,  0,  0,
	 0,  1,  0,
	 0,  0, -1
);
const mat3 kRotationRight = mat3
(
	 0, 0, 1,
	 0, 1, 0,
	-1, 0, 0
);
const mat3 kRotationLeft = mat3
(
	0,  0, -1,
	0,  1,  0,
	1,  0,  0
);
const mat3 kRotationUp = mat3
(
	1,  0,  0,
	0,  0,  1,
	0, -1,  0
);
const mat3 kRotationDown = mat3
(
	1,  0,  0,
	0,  0, -1,
	0,  1,  0
);

// TODO: Figure out some decent way to remove branching
// Could do SH sampling in 9 different passes with different
// shaders... but is that even faster?
float SHSample(int coef_index, vec3 n)
{
	switch (coef_index)
	{
	case 0:
		return kL0m0;
	case 1:
		return kL1m_1 * n.y;
	case 2:
		return kL1m0 * n.z;
	case 3:
		return kL1m1 * n.x;
	case 4:
		return kL2m_2 * n.y * n.x;
	case 5:
		return kL2m_1 * n.y * n.z;
	case 6:
		return kL2m0 * (3 * (n.z * n.z) - 1);
	case 7:
		return kL2m1 * n.x * n.z;
	case 8:
		return kL2m2 * (n.x * n.x - n.y * n.y);
	}
	return 0;
}

vec3 SampleFaces(int coef_index)
{
	// Alpha stores the number of samples taken
	vec4 coefficient = vec4(0.0);

	// +-0.5 to account for cenetered pixels, dw about it...
	vec2 tex_coord = vec2(0.0, (gl_FragCoord.y - 0.5) / floor(probe_count));
	// Do 6 samples per pixel (one for each face) instead of looping over the faces
	// to avoid having to recalculate normals unnecessarily
	// I hope this is actually faster and not just thrashing some GPU sampler cache...
	for (float x = 0.5; x < probe_map_size; x++)
	{
		for (float y = 0.5; y < probe_map_size; y++)
		{
			vec2 sample_point = tex_coord;
			sample_point.x += x / float(probe_map_size * 6);
			sample_point.y += y / float(probe_map_size * probe_count);

			vec2 d = vec2(x / float(probe_map_size), y / float(probe_map_size));
			// Maps [x,y] = [0,0] to the normal [1,1,1]... [x,y] = [probe_map_size] to the normal [-1,-1,1]
			// normal.z is -1 because we're right handed (but not irl...)
			vec3 normal = normalize(vec3(d.xy * 2.0 - 1.0, -1.0));

			vec3 sample_normal = kRotationFront * normal;
			vec3 sample_colour = texture(probe_irradiance, sample_point).rgb;
			coefficient += vec4(SHSample(coef_index, sample_normal) * sample_colour, 1.0);

			sample_point.x += float(probe_map_size) / 6.0;
			sample_normal = kRotationRear * normal;
			sample_colour = texture(probe_irradiance, sample_point).rgb;
			coefficient += vec4(SHSample(coef_index, sample_normal) * sample_colour, 1.0);

			sample_point.x += float(probe_map_size) / 6.0;
			sample_normal = kRotationRight * normal;
			sample_colour = texture(probe_irradiance, sample_point).rgb;
			coefficient += vec4(SHSample(coef_index, sample_normal) * sample_colour, 1.0);

			sample_point.x += float(probe_map_size) / 6.0;
			sample_normal = kRotationLeft * normal;
			sample_colour = texture(probe_irradiance, sample_point).rgb;
			coefficient += vec4(SHSample(coef_index, sample_normal) * sample_colour, 1.0);

			sample_point.x += float(probe_map_size) / 6.0;
			sample_normal = kRotationUp * normal;
			sample_colour = texture(probe_irradiance, sample_point).rgb;
			coefficient += vec4(SHSample(coef_index, sample_normal) * sample_colour, 1.0);

			sample_point.x += float(probe_map_size) / 6.0;
			sample_normal = kRotationDown * normal;
			sample_colour = texture(probe_irradiance, sample_point).rgb;
			coefficient += vec4(SHSample(coef_index, sample_normal) * sample_colour, 1.0);
		}
	}
	return (coefficient.rgb / coefficient.a);
}

void main(void)
{
	const int coef_count = 9;

	int coef_index = int(gl_FragCoord.x);
	int probe_index = int(gl_FragCoord.y);

	frag_colour = vec4(0.0f);
	if (coef_index < 9)
	{
		frag_colour = vec4(SampleFaces(coef_index), 1.0);
	}
	else
	{
		// Will store probe position, eventually, somehow
		frag_colour = vec4(1.0, gl_FragCoord.y / probe_count, 1.0, 1.0);
	}
}
