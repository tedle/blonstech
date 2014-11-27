#version 400

// Ins n outs
in vec2 tex_coord;
layout(origin_upper_left) in vec4 gl_FragCoord;

out vec4 frag_colour;

// Globals
uniform sampler2D diffuse;
uniform vec4 text_colour;
uniform int is_text;

void main(void)
{
	if (is_text == 1)
	{
		float alpha = texture(diffuse, tex_coord).r;
		frag_colour = vec4(text_colour.rgb, text_colour.a * alpha);
	}
	else
	{
		frag_colour = texture(diffuse, tex_coord);
	}
	// Cropping!
	/* if (gl_FragCoord.x < 100)
	{
		float alpha = max(0, (gl_FragCoord.x - 70) / 30);
		frag_colour.a *= alpha;
	} */
}
