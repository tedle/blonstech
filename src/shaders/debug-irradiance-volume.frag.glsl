////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
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

#version 430

// Includes
#include <shaders/math.lib.glsl>

// Ins n outs
in vec3 norm;
in vec3 sample_pos;

out vec4 frag_colour;

// Globals
uniform sampler3D irradiance_volume_px_nx_py_ny;
uniform sampler3D irradiance_volume_pz_nz;

void main(void)
{
    // Irradiance volume stored as ambient cube, reconstruct sky vis from data
    vec3 norm_sq = norm * norm;
    ivec3 is_negative = ivec3(norm.x < 0.0f, norm.y < 0.0f, norm.z < 0.0f);
    vec4 px_nx_py_ny = texture(irradiance_volume_px_nx_py_ny, sample_pos);
    vec2 pz_nz = texture(irradiance_volume_pz_nz, sample_pos).rg;
    float sky_vis = norm_sq.x * px_nx_py_ny[is_negative.x] +
                    norm_sq.y * px_nx_py_ny[is_negative.y + 2] +
                    norm_sq.z * pz_nz[is_negative.z];
    // Visualize as exit irradiance, divide by pi
    sky_vis /= kPi;
    frag_colour = vec4(vec3(sky_vis), 1.0f);
}
