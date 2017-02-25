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

// Determines the intensity of the light on a diffuse surface
float DiffuseTerm(vec3 albedo, vec3 metalness, float NdotV, float NdotL, float LdotH, float roughness)
{
    // Disney diffuse term, modified by the Frostbite team for energy conservation
    // This is already normalized for energy conservation which means we don't have to multiply by 1.0 - fresnel
    // sqrt of the GGX roughness term is perceptively linear which is necessary for this BRDF
    float lin_roughness = sqrt(roughness);
    float energy_bias = mix(0.0, 0.5, lin_roughness);
    float energy_factor = mix(1.0, 1.0 / 1.51, lin_roughness);
    float f0 = 1.0;
    float fd90 = energy_bias + 2.0 * LdotH * LdotH * lin_roughness;
    // Weird custom fresnel term
    float light_scatter = 1.0 + (fd90 - 1.0) * pow(1.0 - NdotL, 5.0);
    float view_scatter = 1.0 + (fd90 - 1.0) * pow(1.0 - NdotV, 5.0);

    return light_scatter * view_scatter * energy_factor;
}

// Determines the probability of incoming rays to be occluded by microfacets during exitance
// Simply put "microfacet self-shadowing"
float SpecularGeometryTerm(float NdotL, float NdotV, float roughness)
{
    // Smith GGX Correlated Geometry term
    // G(v,l,a) = 1 / (1 + lambda(v) + lambda(l))
    // lambda(x) = (-1 + sqrt(1 + a^2(1 - x^2) / x^2)) / 2
    // v = dot(view, normal)
    // l = dot(incident light, normal)
    // a = roughness

    float NdotL2 = NdotL * NdotL;
    float NdotV2 = NdotV * NdotV;
    float a2 = roughness * roughness;

    float lambda_l = (-1.0 + sqrt(1.0 + a2 * (1.0 - NdotL2) / NdotL2)) * 0.5;
    float lambda_v = (-1.0 + sqrt(1.0 + a2 * (1.0 - NdotV2) / NdotV2)) * 0.5;

    float geometry_term = 1.0 / (1.0 + lambda_l + lambda_v);
    return geometry_term;
}

// Determines the probability of incoming rays to be occluded by microfacets during exitance
// Simply put "microfacet self-shadowing"
float SpecularNormalDistributionFunction(float NdotH, float roughness)
{
    // GGX Normal Distribution Function
    // NDF(n,h,a) = a^2 / (pi * ((n.h)^2 * (a^2 - 1) + 1)^2)
    // n = surface normal
    // h = halfway vector normalize(light + view)
    // a = roughness
    //
    // Simplified: x = n.h, m = x^2 * (a^2 - 1) + 1
    //                          x * (x * a^2 - x) + 1
    //                          n.h * (n.h * a^2 - n.h) + 1
    // NDF(n,h,a) = a^2 / (pi * m * m)

    float a2 = roughness * roughness;
    float m = NdotH * (NdotH * a2 - NdotH) + 1;

    float ndf_term = a2 / (3.141592653589793238 * m * m);
    return ndf_term;
}

// Determines the ratio of reflected light vs absorbed light for a material
vec3 FresnelTerm(vec3 metalness, vec3 albedo, float LdotH)
{
    // Fresnel Schlick apprxoimation
    // Dielectrics range from 0.02-0.05
    vec3 f0 = mix(vec3(0.035), albedo, metalness);
    vec3 fresnel = f0 + (1.0 - f0) * pow(1.0 - LdotH, 5.0);
    return fresnel;
}

vec3 SpecularTerm(float roughness, vec3 metalness, vec3 albedo, float NdotH, float NdotL, float NdotV, float LdotH)
{
    // Cook-Torrance specular term
    // Ks = DFG / (4 n.l n.v)
    float D = SpecularNormalDistributionFunction(NdotH, roughness);
    vec3  F = FresnelTerm(metalness, albedo, LdotH);
    float G = SpecularGeometryTerm(NdotL, NdotV, roughness);

    // Avoid division by 0
    vec3 specular_term = D * F * G / (4.0 * NdotL * NdotV + 1e-5);
    return specular_term;
}