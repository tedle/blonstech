#version 400

// Ins n outs
in vec2 tex_coord;
in vec3 norm;

out vec4 frag_colour;

// Globals
uniform sampler2D shader_texture;

void main(void)
{
    vec4 texture_col = texture(shader_texture, tex_coord);
    //frag_colour = texture_col;
    frag_colour = vec4(norm, 1.0);
}