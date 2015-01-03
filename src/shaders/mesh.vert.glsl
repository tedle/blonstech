#version 400

// Ins n outs
in vec3 input_pos;
in vec2 input_uv;
in vec3 input_norm;
in vec3 input_tan;
in vec3 input_bitan;

out vec2 tex_coord;
out mat3 norm;

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
    norm = transpose(mat3(input_tan, input_bitan, input_norm));
}
