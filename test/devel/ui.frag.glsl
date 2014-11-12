#version 400

// Ins n outs
in vec2 tex_coord;

out vec4 frag_colour;

// Globals
uniform sampler2D diffuse;
uniform vec3 text_colour;

void main(void)
{
    float alpha = texture(diffuse, tex_coord).r;
    frag_colour = vec4(text_colour.rgb, alpha);
}
