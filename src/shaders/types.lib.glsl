struct DirectionalLight
{
    // Used as uniforms for currently so vec3 is fine
    vec3 dir;
    vec3 colour;
    float luminance;
};

struct Probe
{
    int id;
    // vec3 is secretly the size of vec4 in std430, don't use it!!!!!!!!!!!
    float pos[3];
    float cube_coeffs[6][3]; // 6 directions, 3 colour channels
    float sh_coeffs[9];
};

struct SHColourCoeffs
{
    float r[9];
    float g[9];
    float b[9];
};