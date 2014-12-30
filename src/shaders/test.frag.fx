// Globals
Texture2D shader_texture;
SamplerState sample_method;

// Types
struct PixelInput
{
    float4 pos : SV_POSITION;
    float4 tex : TEXCOORD0;
};

float4 FragShader(PixelInput input) : SV_TARGET
{
    float4 colour;
    colour = shader_texture.Sample(sample_method, input.tex);

    return colour;
}
