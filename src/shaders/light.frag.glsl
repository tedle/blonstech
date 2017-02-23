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
#include <shaders/colour.lib.glsl>
#include <shaders/types.lib.glsl>
#include <shaders/math.lib.glsl>
#include <shaders/pbr.lib.glsl>

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
uniform float sky_luminance;
uniform float exposure;
uniform sampler3D irradiance_volume_px;
uniform sampler3D irradiance_volume_nx;
uniform sampler3D irradiance_volume_py;
uniform sampler3D irradiance_volume_ny;
uniform sampler3D irradiance_volume_pz;
uniform sampler3D irradiance_volume_nz;
uniform float roughness;
uniform vec3 metalness;

vec3 AmbientDiffuse(vec4 pos, vec3 normal)
{
    // Irradiance volume stored as ambient cube, reconstruct indirect lighting from data
    vec4 irradiance_sample_pos = inv_irradiance_matrix * pos;
    irradiance_sample_pos /= irradiance_sample_pos.w;
    vec3 ambient_cube[6];
    // We do a lot of ugly busywork to cut texture fetches down in half because these are dependent texture fetches and wow those are expensive!!!
    // TODO: Shove indirect lighting computation into a half-res bilaterally-upsampled buffer? This currently adds 0.6ms
    bvec3 is_positive = bvec3(normal.x > 0.0, normal.y > 0.0, normal.z > 0.0);
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
    return SampleAmbientCube(ambient_cube, normal);
}

vec3 Diffuse(vec4 pos, vec3 albedo, vec3 metalness, vec3 surface_normal, vec3 light_visibility, float NdotV, float NdotL, float LdotH, float roughness)
{
    // For the one light we currently have
    vec3 diffuse = vec3(DiffuseTerm(albedo, metalness, NdotV, NdotL, LdotH, roughness));
    diffuse = diffuse * light_visibility * sun.luminance * sun.colour * NdotL;
    // After all other lights are applied
    vec3 ambient = AmbientDiffuse(pos, surface_normal);
    diffuse += ambient;
    // Modulate by surface colour
    diffuse *= albedo;
    // Metals dont have diffuse light
    diffuse *= 1.0 - metalness;
    // Account for conversion from irradiance to radiant exitance
    return diffuse / kPi;
}

vec3 Specular(vec3 metalness, vec3 albedo, vec3 direct, float NdotH, float NdotV, float NdotL, float LdotH, float roughness)
{
    // Determine specular term
    vec3 specular = SpecularTerm(roughness, metalness, albedo, NdotH, NdotL, NdotV, LdotH);
    specular = specular * sun.luminance * sun.colour * direct * NdotL;
    // Division by pi already accounted for in BRDF
    return specular;
}

vec3 SkyColour(vec3 view_dir)
{
    float direction_coeffs[9];
    SHProjectDirection3(-view_dir, direction_coeffs);
    vec3 sky_colour = vec3(SHDot3(direction_coeffs, sh_sky_colour.r),
                           SHDot3(direction_coeffs, sh_sky_colour.g),
                           SHDot3(direction_coeffs, sh_sky_colour.b));
    sky_colour *= sky_luminance;
    return FilmicTonemap(sky_colour * exposure);
}

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
        frag_colour = vec4(SkyColour(view_dir), 1.0);
        return;
    }

    // Get the albedo
    vec3 albedo = texture(albedo, tex_coord).rgb;
    // Get the direct lighting value
    vec3 direct = texture(direct_light, tex_coord).rgb;
    // Get the surface normal
    vec3 surface_normal = normalize(texture(normal, tex_coord).rgb * 2.0 - 1.0);

    // Pre-computed and re-used in various lighting calculations
    vec3 halfway = normalize(-sun.dir + view_dir);
    float NdotH = max(dot(surface_normal, halfway), 0.0);
    float NdotL = max(dot(surface_normal, -sun.dir), 0.0);
    float NdotV = max(dot(surface_normal, view_dir), 0.0);
    float LdotH = max(dot(-sun.dir, halfway), 0.0);

    vec3 diffuse = Diffuse(pos, albedo, metalness, surface_normal, direct, NdotV, NdotL, LdotH, roughness);
    vec3 specular = Specular(metalness, albedo, direct, NdotH, NdotV, NdotL, LdotH, roughness);

    vec3 surface_colour = diffuse + specular;

    surface_colour = FilmicTonemap(surface_colour * exposure);

    // Uncomment to see direct light only
    //surface_colour *= 0.000001;
    //surface_colour += vec3(direct * NdotL);

    // Uncomment to see ambient light only
    //surface_colour *= 0.000001;
    //surface_colour += vec3(ambient);

    // Uncomment to see specular only
    //surface_colour *= 0.000001;
    //surface_colour += FilmicTonemap(vec3(specular) * exposure);

    // Final composite
    surface_colour = GammaEncode(surface_colour);
    frag_colour = vec4(surface_colour, 1.0);
}
