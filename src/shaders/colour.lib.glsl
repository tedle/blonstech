vec3 GammaEncode(vec3 colour)
{
    return pow(colour, vec3(1/2.2));
}

vec3 GammaDecode(vec3 colour)
{
    return pow(colour, vec3(2.2));
}

vec3 FilmicTonemap(vec3 colour)
{
    // Approximation of ACES mapping, better fitted for lighting in games
    // See: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((colour*(a*colour + b)) / (colour*(c*colour + d) + e), 0.0, 1.0);
}