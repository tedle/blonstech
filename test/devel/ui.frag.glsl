#version 400

// Ins n outs
in vec2 tex_coord;

out vec4 frag_colour;

// Globals
uniform sampler2D diffuse;
uniform vec3 text_colour;
uniform int is_text;

void main(void)
{
	if (is_text == 1)
	{
		float alpha = texture(diffuse, tex_coord).r;
		frag_colour = vec4(text_colour.rgb, alpha);
	}
	else
	{
		frag_colour = texture(diffuse, tex_coord);
	}
}
