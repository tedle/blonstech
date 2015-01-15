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

void SampleFace(int face_index)
{
	// +-0.5 to account for cenetered pixels, dw about it...
	vec2 tex_coord = vec2(float(face_index) / 6.0, (gl_FragCoord.y - 0.5) / floor(probe_count));
	for (float x = 0.5; x < probe_map_size; x++)
	{
		for (float y = 0.5; y < probe_map_size; y++)
		{
			vec2 sample_point = tex_coord;
			sample_point.x += float(x) / float(probe_map_size * 6);
			sample_point.y += float(y) / float(probe_map_size * probe_count);
			// TODO: GAMMA!!!!!!!!
			frag_colour += texture(probe_irradiance, sample_point);
		}
	}
}

void main(void)
{
	const int coef_count = 9;

	int coef_index = int(gl_FragCoord.x);
	int probe_index = int(gl_FragCoord.y);

	frag_colour = vec4(0.0f);
	if (coef_index < 9)
	{
		SampleFace(0);
		SampleFace(1);
		SampleFace(2);
		SampleFace(3);
		SampleFace(4);
		SampleFace(5);
		frag_colour /= probe_map_size * probe_map_size;
	}
	else
	{
		// Will store probe position, eventually, somehow
		frag_colour = vec4(1.0, gl_FragCoord.y / probe_count, 1.0, 1.0);
	}
}
