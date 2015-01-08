#version 400

// Ins n outs
in vec2 tex_coord;

out vec4 frag_colour;

// Globals
uniform mat4 inv_proj_view_matrix;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D depth;

void main(void)
{
	vec4 pos = vec4(tex_coord.x,
					tex_coord.y + 1, // Invert Y because FBOs are top-left origin
					texture(depth, tex_coord).r,
					1.0);
	pos = inv_proj_view_matrix * (pos * 2.0 - 1.0);
	pos /= pos.w;

	// Temporarily stop these uniforms from being optimized out
	vec4 a = texture(normal, tex_coord) * 0.001;
	vec4 b = texture(albedo, tex_coord) * 0.001;
	frag_colour = a + b;
	frag_colour *= 0.0000001;

	frag_colour += pos;
}