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
#include <shaders/types.lib.glsl>
#include <shaders/math.lib.glsl>

// Ins n outs
in vec2 tex_coord;

out vec4 frag_colour;

// Globals
uniform mat4 inv_vp_matrix;
uniform mat4 inv_irradiance_matrix;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D depth;
uniform sampler2D direct_light;
uniform DirectionalLight sun;
uniform SHColourCoeffs sh_sky_colour;
uniform sampler3D irradiance_volume_px;
uniform sampler3D irradiance_volume_nx;
uniform sampler3D irradiance_volume_py;
uniform sampler3D irradiance_volume_ny;
uniform sampler3D irradiance_volume_pz;
uniform sampler3D irradiance_volume_nz;

// Used to give stronger specular when light bounces at shallower angles
// refraction_index of 1.0 gives a fresnel of about 0.058~ and 0.1 gives a coef of about 0.72
// Dielectrics are usually 0.02-0.05 fresnel, so a refraction index of 1 is about good for that
const float refraction_index = 1.0;
const float extinction_coef = 0.5;
const float fresnel_coef = (pow(refraction_index - 1, 2) + pow(extinction_coef, 2)) / (pow(refraction_index + 1, 2) + pow(extinction_coef, 2));
// Pop this somewhere between 0.1-100,000 idk
const float gloss = 20.0;

void main(void)
{
    float depth_sample = texture(depth, tex_coord).r;
    // World coordinates of the pixel we're rendering
    vec4 pos = vec4(tex_coord.x,
                    tex_coord.y,
                    depth_sample,
                    1.0);
    pos = inv_vp_matrix * (pos * 2.0 - 1.0);
    pos /= pos.w;

    // For directional lights
    vec4 eye_pos = inv_vp_matrix * vec4(0.0, 0.0, 0.0, 1.0);
    eye_pos /= eye_pos.w;
    vec3 view_dir = normalize(eye_pos.xyz - pos.xyz);

    if (depth_sample == 1.0)
    {
        float direction_coeffs[9];
        SHProjectDirection3(-view_dir, direction_coeffs);
        vec3 sky_colour = vec3(SHDot3(direction_coeffs, sh_sky_colour.r),
                               SHDot3(direction_coeffs, sh_sky_colour.g),
                               SHDot3(direction_coeffs, sh_sky_colour.b));
        frag_colour = vec4(sky_colour, 1.0);
        return;
    }

    vec3 surface_normal = normalize(texture(normal, tex_coord).rgb * 2.0 - 1.0);
    vec3 halfway = normalize(-sun.dir + view_dir);

    // Higher exponent used because this is blinn-phong (blinn-phong * 4 ~= phong)
    vec3 specular = vec3(pow(clamp(dot(halfway, surface_normal), 0.0, 1.0), gloss));

    float light_angle = dot(surface_normal, -sun.dir);
    // Black out surfaces not facing a light
    if (light_angle < 0.0)
    {
        specular = vec3(0.0);
    }

    // Get the direct lighting value
    vec3 direct = texture(direct_light, tex_coord).rgb;

    // Helps ensure outgoing light is never greater than incoming (real!)
    float specular_normalization = ((gloss + 2) / 8);

    // Linear blend specular with camera angle, based on fresnel coefficient
    float fresnel = fresnel_coef + (1 - fresnel_coef) * pow(1.0 - dot(view_dir, halfway), 5.0);
    specular *= fresnel;
    specular *= specular_normalization;
    // This has the N.L pre-applied
    specular *= direct;

    // Irradiance volume stored as ambient cube, reconstruct indirect lighting from data
    vec4 irradiance_sample_pos = inv_irradiance_matrix * pos;
    irradiance_sample_pos /= irradiance_sample_pos.w;
    vec3 ambient_cube[6];
    // We do a lot of ugly busywork to cut texture fetches down in half because these are dependent texture fetches and wow those are expensive!!!
    // TODO: Shove indirect lighting computation into a half-res bilaterally-upsampled buffer? This currently adds 0.6ms
    bvec3 is_positive = bvec3(surface_normal.x > 0.0, surface_normal.y > 0.0, surface_normal.z > 0.0);
    ivec3 cube_indices = ivec3(is_positive.x ? kPositiveX : kNegativeX,
                               is_positive.y ? kPositiveY : kNegativeY,
                               is_positive.z ? kPositiveZ : kNegativeZ);
    ambient_cube[cube_indices.x] = is_positive.x ?
                                       vec3(texture(irradiance_volume_px, irradiance_sample_pos.xyz).rgb) :
                                       vec3(texture(irradiance_volume_nx, irradiance_sample_pos.xyz).rgb);
    ambient_cube[cube_indices.y] = is_positive.y ?
                                       vec3(texture(irradiance_volume_py, irradiance_sample_pos.xyz).rgb) :
                                       vec3(texture(irradiance_volume_ny, irradiance_sample_pos.xyz).rgb);
    ambient_cube[cube_indices.z] = is_positive.z ?
                                       vec3(texture(irradiance_volume_pz, irradiance_sample_pos.xyz).rgb) :
                                       vec3(texture(irradiance_volume_nz, irradiance_sample_pos.xyz).rgb);
    vec3 ambient = SampleAmbientCube(ambient_cube, surface_normal);
    // This pi divide should actually be done during surface colour calculation but the whole BRDF is a mess right now
    // Will be fixed during proper PBR pass
    ambient /= kPi;
    vec3 diffuse = (direct * sun.colour) + ambient;

    vec3 surface_colour = texture(albedo, tex_coord).rgb;
    surface_colour *= diffuse;
    surface_colour += specular;

    // Uncomment to see ambient light only
    //surface_colour *= 0.000001;
    //surface_colour += vec3(ambient);

    // Uncomment to see specular only
    //surface_colour *= 0.000001;
    //surface_colour += vec3(specular);

    // Final composite
    surface_colour = GammaEncode(surface_colour);
    frag_colour = vec4(surface_colour, 1.0);
}
