#version 400

// Ins n outs
in vec3 colour;

out vec4 output_col;

void main(void)
{
    output_col = vec4(colour, 1.0f);
}