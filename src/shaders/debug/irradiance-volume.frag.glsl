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

#version 430

// Includes
#include <shaders/lib/colour.lib.glsl>
#include <shaders/lib/math.lib.glsl>

// Ins n outs
in vec3 norm;
in vec3 sample_pos;

out vec4 frag_colour;

// Globals
uniform sampler3D irradiance_volume_px;
uniform sampler3D irradiance_volume_nx;
uniform sampler3D irradiance_volume_py;
uniform sampler3D irradiance_volume_ny;
uniform sampler3D irradiance_volume_pz;
uniform sampler3D irradiance_volume_nz;
uniform float exposure;

void main(void)
{
    // Irradiance volume stored as ambient cube, reconstruct indirect lighting from data
    vec3 ambient_cube[6] = vec3[6](
        vec3(texture(irradiance_volume_px, sample_pos).rgb),
        vec3(texture(irradiance_volume_nx, sample_pos).rgb),
        vec3(texture(irradiance_volume_py, sample_pos).rgb),
        vec3(texture(irradiance_volume_ny, sample_pos).rgb),
        vec3(texture(irradiance_volume_pz, sample_pos).rgb),
        vec3(texture(irradiance_volume_nz, sample_pos).rgb)
    );
    vec3 ambient_light = SampleAmbientCube(ambient_cube, norm);
    // Visualize as exit irradiance, divide by pi
    ambient_light /= kPi;
    ambient_light = GammaEncode(FilmicTonemap(ambient_light * exposure));
    frag_colour = vec4(ambient_light, 1.0f);
}
