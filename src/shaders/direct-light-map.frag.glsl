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
in vec3 normal;

out vec4 frag_colour;

// Globals
uniform mat4 light_vp_matrix;
uniform sampler2D light_depth;

struct DirectionalLight
{
	vec3 dir;
};
uniform DirectionalLight sun;

float PercentageList(vec4 pos)
{
	vec4 light_pos = light_vp_matrix * pos;

	// Shouldn't need this I think? Cus orthographic matrix...
	// Maybe spot lights use perspective tho?? duno!
	// TODO: Check if this is needed
	light_pos /= light_pos.w;

	// Adjust for [0,1] space
	light_pos = (light_pos + 1.0) / 2.0;

	// Variance shadow maping
	vec2 moments = texture(light_depth, light_pos.xy).rg;

	// (local_depth - mean_depth)
	float local_delta = light_pos.z - moments.r;

	// o^2 = M2 - M1^2
	float variance = moments.g - (moments.r * moments.r);
	float bias = 0.00001;
	variance = max(variance, bias);

	// P = o^2 / (o^2 + (local_depth - mean_depth)^2) = probability of surface being lit
	float p_max = variance / (variance + (local_delta * local_delta));

	// p_max is invalid if the measured depth is greater than the local depth
	// Use a smoothstep function to blend to 1 as we approach this
	// Increases light bleeding, but looks way less gritty
	float p = smoothstep(light_pos.z - 0.01, light_pos.z - 0.005, moments.x);
	// Use a smoothstep on pmax to decrease lightbleeding, both mitigating the bleed
	// incurred by the p smoothing as well as the natural bleed induced by VSM
	p_max = smoothstep(0.2, 1.0, p_max);
	// The actual percent amount that the fragment is affected by the light source
	return max(p, p_max);
}

void main(void)
{
	float lit = PercentageList(vec4(pos, 1.0));

	float light_angle = clamp(dot(normal, -sun.dir), 0.0, 1.0);

	frag_colour = vec4(vec3(lit * light_angle), 1.0);
}
