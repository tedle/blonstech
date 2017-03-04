////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2017 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

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
    int brickfactor_range_start;
    int brickfactor_range_end;
};

struct Surfel
{
    int nearest_probe;
    float pos[3];
    float normal[3];
    float albedo[3];
    float radiance[3];
};

struct SurfelBrick
{
    int surfel_range_start;
    int surfel_count;
    float radiance[3];
};

struct SurfelBrickFactor
{
    int brick_id;
    float brick_weight[6];
};

struct SHColourCoeffs
{
    float r[9];
    float g[9];
    float b[9];
};