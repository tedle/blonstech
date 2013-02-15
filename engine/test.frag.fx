// Types
struct PixelInputType
{
    float4 pos : SV_POSITION;
    float4 colour : COLOUR;
};

float4 FragShader(PixelInputType input) : SV_TARGET
{
    input.colour.r = 1.0f;
    input.colour.a = 1.0f;
    return input.colour;
}
