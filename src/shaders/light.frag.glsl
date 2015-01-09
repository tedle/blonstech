#version 400

// Ins n outs
in vec2 tex_coord;

out vec4 frag_colour;

// Globals
uniform mat4 inv_proj_view_matrix;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D depth;

// Used to give stronger specular when light bounces at shallower angles
float refraction_index = 0.5;
float extinction_coef = 0.5;
float fresnel_coef = (pow(refraction_index - 1, 2) + pow(extinction_coef, 2)) / (pow(refraction_index + 1, 2) + pow(extinction_coef, 2));

// Noon
//vec3 light_dir = normalize(vec3(-10.0, -20.0, -5.0));
// Sunset
vec3 light_dir = normalize(vec3(-10.0, -2.0, -5.0));

void main(void)
{
	// World coordinates of the pixel we're rendering
	vec4 pos = vec4(tex_coord.x,
					tex_coord.y + 1, // Invert and shift Y because FBOs are top-left origin
					texture(depth, tex_coord).r,
					1.0);
	pos = inv_proj_view_matrix * (pos * 2.0 - 1.0);
	pos /= pos.w;

	// For directional lights
	vec4 eye_pos = inv_proj_view_matrix * vec4(0.0, 0.0, 0.0, 1.0);
	eye_pos /= eye_pos.w;
	vec3 view_dir = normalize(eye_pos.xyz - pos.xyz);

	vec3 surface_normal = normalize(texture(normal, tex_coord).rgb * 2.0 - 1.0);
	vec3 half = normalize(-light_dir + view_dir);

	// Higher exponent used because this is blinn-phong (blinn-phong * 4 ~= phong)
	float specular = pow(clamp(dot(half, surface_normal), 0.0, 1.0), 20.0);

	float light_angle = dot(surface_normal, -light_dir);
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
	float diffuse = clamp(dot(-light_dir, surface_normal), 0.0, 1.0);
	// Ambient lighting
	diffuse += 0.1;

	vec3 surface_colour = pow(texture(albedo, tex_coord).rgb, vec3(2.2));
	surface_colour *= diffuse;
	surface_colour += specular;

	surface_colour = pow(surface_colour, vec3(1/2.2));
	frag_colour = vec4(surface_colour, 1.0);
}