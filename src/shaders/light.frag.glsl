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
uniform mat4 inv_vp_matrix;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D depth;

// Used to give stronger specular when light bounces at shallower angles
const float refraction_index = 0.5;
const float extinction_coef = 0.5;
const float fresnel_coef = (pow(refraction_index - 1, 2) + pow(extinction_coef, 2)) / (pow(refraction_index + 1, 2) + pow(extinction_coef, 2));

struct DirectionalLight
{
	vec3 dir;
	vec3 colour;
	vec3 ambient;
	vec3 specular;
};
uniform DirectionalLight sun;

void main(void)
{
	// World coordinates of the pixel we're rendering
	vec4 pos = vec4(tex_coord.x,
					tex_coord.y + 1, // Invert and shift Y because FBOs are top-left origin
					texture(depth, tex_coord).r,
					1.0);
	pos = inv_vp_matrix * (pos * 2.0 - 1.0);
	pos /= pos.w;

	// For directional lights
	vec4 eye_pos = inv_vp_matrix * vec4(0.0, 0.0, 0.0, 1.0);
	eye_pos /= eye_pos.w;
	vec3 view_dir = normalize(eye_pos.xyz - pos.xyz);

	vec3 surface_normal = normalize(texture(normal, tex_coord).rgb * 2.0 - 1.0);
	vec3 half = normalize(-sun.dir + view_dir);

	// Higher exponent used because this is blinn-phong (blinn-phong * 4 ~= phong)
	float specular = pow(clamp(dot(half, surface_normal), 0.0, 1.0), 20.0);

	float light_angle = dot(surface_normal, -sun.dir);
	// Black out surfaces not facing a light
	if (light_angle < 0.1)
	{
		specular *= light_angle * 10;
		if (light_angle < 0.0)
		{
			specular = 0;
		}
	}

	// Linear blend specular with camera angle, based on fresnel coefficient
	float fresnel = fresnel_coef + (1 - fresnel_coef) * pow(1.0 - dot(view_dir, half), 5.0);
	specular *= fresnel;

	// Diffuse lighting (temporary)
	vec3 diffuse = sun.colour * clamp(light_angle, 0.0, 1.0);
	// Ambient lighting
	diffuse += sun.ambient;

	vec3 surface_colour = pow(texture(albedo, tex_coord).rgb, vec3(2.2));
	surface_colour *= diffuse;
	surface_colour += specular * sun.specular;

	surface_colour = pow(surface_colour, vec3(1/2.2));
	frag_colour = vec4(surface_colour, 1.0);
}