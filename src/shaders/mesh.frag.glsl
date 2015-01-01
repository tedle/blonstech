#version 400

// Ins n outs
in vec2 tex_coord;
in vec3 norm;

out vec4 frag_colour;
out vec4 norm_colour;

// Globals
uniform sampler2D diffuse;

void main(void)
{
    norm_colour = vec4(norm, 1.0);
    frag_colour = texture(diffuse, tex_coord);
}