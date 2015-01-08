#version 400

// Ins n outs
in vec2 tex_coord;
layout(origin_upper_left) in vec4 gl_FragCoord;

out vec4 frag_colour;

// Globals
uniform sampler2D skin;
uniform vec4 text_colour;
uniform int is_text;
uniform vec4 crop;
uniform int feather;

void main(void)
{
	if (is_text == 1)
	{
		float alpha = texture(skin, tex_coord).r;
		frag_colour = vec4(text_colour.rgb, text_colour.a * alpha);
	}
	else
	{
		frag_colour = texture(skin, tex_coord);
	}
	// Cropping!
	if (feather > 0)
	{
		if (crop.z != 0)
		{
			if (gl_FragCoord.x < crop.x + feather)
			{
				float alpha = max(0, (gl_FragCoord.x - crop.x) / feather);
				frag_colour.a *= alpha;
			}
			else if (gl_FragCoord.x > crop.x + crop.z - feather)
			{
				float alpha = max(0, (crop.x + crop.z - gl_FragCoord.x) / feather);
				frag_colour.a *= alpha;
			}
		}
		if (crop.w != 0)
		{
			if (gl_FragCoord.y < crop.y + feather)
			{
				float alpha = max(0, (gl_FragCoord.y - crop.y) / feather);
				frag_colour.a *= alpha;
			}
			else if (gl_FragCoord.y > crop.y + crop.w - feather)
			{
				float alpha = max(0, (crop.y + crop.w - gl_FragCoord.y) / feather);
				frag_colour.a *= alpha;
			}
		}
	}
}
