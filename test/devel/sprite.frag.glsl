#version 400

// Ins n outs
in vec2 tex_coord;

out vec4 frag_colour;

// Globals
uniform sampler2D diffuse;

void main(void)
{
    frag_colour = texture(diffuse, tex_coord);
}
