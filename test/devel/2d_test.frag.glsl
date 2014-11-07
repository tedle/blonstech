#version 400

// Ins n outs
in vec2 tex_coord;

out vec4 frag_colour;

// Globals
uniform sampler2D diffuse;

void main(void)
{
    vec4 texture_col = texture(diffuse, tex_coord);
    frag_colour = texture_col;
    //frag_colour = vec4(1.0, 0.0, 0.0, 1.0);
}
