struct Probe
{
    int id;
    // vec3 is secretly the size of vec4, don't use it!!!!!!!!!!!
    float pos[3];
    float sh_coeffs[9];
};