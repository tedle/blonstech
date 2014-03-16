#version 400

// Ins n outs
in vec3 input_pos;
in vec3 input_col;

out vec3 colour;

// Globals
uniform Mat4 world_matrix;
uniform Mat4 view_matrix;
uniform Mat4 proj_matrix;

void main(void)
{
    gl_Position = world_matrix * vec4(input_pos, 1.0f);
    gl_Position = view_matrix * gl_Position;
    gl_Position = proj_matrix * gl_Position;

    colour = input_col;
}