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
#include <shaders/gamma.lib.glsl>

// Ins n outs
in vec2 tex_coord;

out vec4 frag_colour;

// Globals
uniform sampler2D probe_albedo;
uniform sampler2D probe_texmap;
uniform sampler2D direct_lightmap;
uniform sampler2D indirect_lightmap;
uniform vec3 sky_colour;

void main(void)
{
    vec3 albedo = texture(probe_albedo, tex_coord).rgb;
    vec3 light_coord = texture(probe_texmap, tex_coord).rgb;

    // Should only ever be 1 or 0 but doesnt hurt to be safe
    if (light_coord.b > 0.5)
    {
        // Sky should be about 1/5th as strong as sun, i think
        const float sky_correction = 0.2;
        frag_colour = vec4(GammaDecode(sky_colour) * sky_correction, 1.0);
        return;
    }
    vec4 direct_light_full = texture(direct_lightmap, light_coord.xy);
    vec3 direct_light_colour = clamp(direct_light_full.rgb / direct_light_full.a, vec3(0), vec3(1));
    vec4 indirect_light_full = texture(indirect_lightmap, light_coord.xy);
    vec3 indirect_light_colour = clamp(indirect_light_full.rgb / indirect_light_full.a, vec3(0), vec3(1));

    vec3 diffuse = albedo * (direct_light_colour + indirect_light_colour);

    frag_colour = vec4(diffuse, 1.0);
}