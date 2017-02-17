struct Probe
{
    int id;
    // vec3 is secretly the size of vec4, don't use it!!!!!!!!!!!
    float pos[3];
    float cube_coeffs[6][3]; // 6 directions, 3 colour channels
    float sh_coeffs[9];
};