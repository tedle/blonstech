vec3 GammaEncode(vec3 colour)
{
    return pow(colour, vec3(1/2.2));
}

vec3 GammaDecode(vec3 colour)
{
    return pow(colour, vec3(2.2));
}