#version 400

// Ins n outs
in vec2 tex_coord;
in mat3 norm;

out vec4 frag_colour;
out vec4 norm_colour;
out vec4 debug_colour;

// Globals
uniform sampler2D diffuse;
uniform sampler2D normal;

void main(void)
{
	// Diffuse
    frag_colour = texture(diffuse, tex_coord);

	// Normal
	vec3 norm_map = (texture(normal, tex_coord).rgb * 2) - 1;
	norm_colour = vec4(((norm_map * norm) + 1) / 2, 1.0);

	// Debug out (UV coords)
	debug_colour = vec4(tex_coord, 0.0, 1.0);
}