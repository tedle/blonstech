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
#include <shaders/lib/types.lib.glsl>
#include <shaders/lib/math.lib.glsl>
#include <shaders/lib/pbr.lib.glsl>
#include <shaders/lib/sky.lib.glsl>

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
uniform sampler2D brdf_lut;
uniform samplerCube local_specular_probe;
uniform int max_mip_level;
uniform float roughness;
uniform vec3 metalness;

// Note: This function is also used in shaders/specular-probe-relight.frag.glsl
// Consider the implications of any changes here maybe needing to be included in
// that shader as well
vec3 AmbientDiffuse(vec4 pos, vec3 normal, vec3 albedo, vec2 diffuse_brdf)
{
    // Irradiance volume stored as ambient cube, reconstruct indirect lighting from data
    vec4 irradiance_sample_pos = inv_irradiance_matrix * pos;
    irradiance_sample_pos /= irradiance_sample_pos.w;
    vec3 ambient_cube[6];
    // We do a lot of ugly busywork to cut texture fetches down in half because these are dependent texture fetches and wow those are expensive!!!
    // TODO: Shove indirect lighting computation into a half-res bilaterally-upsampled buffer? Or possibly
    // do this step during G-buffer creation as that would make for independent fetches. This currently adds 0.6ms
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
    vec3 ambient = SampleAmbientCube(ambient_cube, normal);
    // Modulate by surface colour and diffuse term
    return ambient * albedo * (diffuse_brdf.x + albedo * diffuse_brdf.y);
}

vec3 Diffuse(vec4 pos, vec3 albedo, vec3 metalness, vec3 surface_normal, vec3 light_visibility, vec2 diffuse_brdf,
             float NdotV, float NdotL, float LdotH, float LdotV, float roughness)
{
    // For the one light we currently have
    vec3 diffuse = DiffuseTermGGX(albedo, NdotV, NdotL, LdotH, LdotV, roughness);
    diffuse = diffuse * light_visibility * sun.luminance * sun.colour * NdotL;
    // After all other lights are applied
    diffuse += AmbientDiffuse(pos, surface_normal, albedo, diffuse_brdf);
    // Metals dont have diffuse light
    diffuse *= 1.0 - metalness;
    // Account for conversion from irradiance to radiance
    return diffuse / kPi;
}

vec3 AmbientSpecular(vec3 metalness, vec3 albedo, vec3 surface_normal, vec3 view, vec2 brdf, float roughness)
{
    vec3 sample_dir = SpecularDominantDirectionGGX(surface_normal, reflect(view, surface_normal), roughness);
    float lod = RoughnessToMipLevel(roughness, max_mip_level);
    vec3 light = textureLod(local_specular_probe, sample_dir, lod).rgb;
    vec3 f0 = Fresnel0Term(metalness, albedo);
    return light * (f0 * brdf.x + brdf.y);
}

vec3 Specular(vec3 metalness, vec3 albedo, vec3 direct, vec3 surface_normal, vec3 view, vec2 specular_brdf,
              float NdotH, float NdotV, float NdotL, float LdotH, float roughness)
{
    // TODO: Modulate by specular occlusion (see Frostbite PBR paper) once we have an accessible AO term
    // Determine specular term
    vec3 specular = SpecularTerm(roughness, metalness, albedo, NdotH, NdotL, NdotV, LdotH);
    specular = specular * sun.luminance * sun.colour * direct * NdotL;
    specular += AmbientSpecular(metalness, albedo, surface_normal, view, specular_brdf, roughness);
    // TODO: double check the ambient specular is multiplied by f0 and NOT the fresnel term itself!
    // Division by pi already accounted for in BRDF
    return specular;
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
    vec3 view_dir = normalize(pos.xyz - eye_pos.xyz);

    if (depth_sample == 1.0)
    {
        vec3 sky_colour = SkyLight(view_dir, sh_sky_colour, sky_luminance);
        frag_colour = vec4(GammaEncode(FilmicTonemap(sky_colour * exposure)), 1.0);
        return;
    }

    // Get the albedo
    vec3 albedo = texture(albedo, tex_coord).rgb;
    // Get the direct lighting value
    vec3 direct = texture(direct_light, tex_coord).rgb;
    // Get the surface normal
    vec3 surface_normal = normalize(texture(normal, tex_coord).rgb * 2.0 - 1.0);

    // Pre-computed and re-used in various lighting calculations
    vec3 halfway = normalize(-sun.dir - view_dir);
    float NdotH = max(dot(surface_normal, halfway), 0.0);
    float NdotL = max(dot(surface_normal, -sun.dir), 0.0);
    float NdotV = max(dot(surface_normal, -view_dir), 0.0);
    float LdotH = max(dot(-sun.dir, halfway), 0.0);
    float LdotV = max(dot(-sun.dir, -view_dir), 0.0);

    vec4 preintegrated_brdf = texture(brdf_lut, vec2(NdotV, roughness));
    vec3 diffuse = Diffuse(pos, albedo, metalness, surface_normal, direct, preintegrated_brdf.ba, NdotV, NdotL, LdotH, LdotV, roughness);
    vec3 specular = Specular(metalness, albedo, direct, surface_normal, view_dir, preintegrated_brdf.rg, NdotH, NdotV, NdotL, LdotH, roughness);

    vec3 surface_colour = diffuse + specular;

    surface_colour = FilmicTonemap(surface_colour * exposure);

    // Uncomment to see direct light only
    //surface_colour *= 0.000001;
    //surface_colour += vec3(direct * NdotL);

    // Uncomment to see ambient light only
    //surface_colour *= 0.000001;
    //surface_colour += FilmicTonemap((AmbientDiffuse(pos, surface_normal) / kPi) * exposure);

    // Uncomment to see direct specular only
    //surface_colour *= 0.000001;
    //surface_colour += FilmicTonemap(vec3(specular) * exposure);

    // Uncomment to see reflections only
    //surface_colour *= 0.000001f;
    //surface_colour += texture(local_specular_probe, reflect(view_dir, surface_normal)).rgb;

    // Final composite
    surface_colour = GammaEncode(surface_colour);
    frag_colour = vec4(surface_colour, 1.0);
}
