#version 400

// Ins n outs
in vec2 tex_coord;
in vec3 norm;

out vec4 frag_colour;
out vec4 norm_colour;

// Globals
uniform sampler2D diffuse;
uniform sampler2D normal;

void main(void)
{
    norm_colour = texture(normal, tex_coord);
    frag_colour = texture(diffuse, tex_coord);
}