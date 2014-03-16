#version 400

// Ins n outs
in vec3 input_pos;
in vec2 input_uv;

out vec2 tex_coord;

// Globals
uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    gl_Position = world_matrix * vec4(input_pos, 1.0);
    gl_Position = view_matrix * gl_Position;
    gl_Position = proj_matrix * gl_Position;

    tex_coord = input_uv;
}