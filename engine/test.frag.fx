// Types
struct PixelInputType
{
    float4 pos : SV_POSITION;
    float4 colour : COLOUR;
};

float4 FragShader(PixelInputType input) : SV_TARGET
{
    return input.colour;
}
