// Globals
cbuffer MatrixBuffer
{
    matrix world_matrix;
    matrix view_matrix;
    matrix projection_matrix;
};

// Typedefs
struct VertexInput
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInput VertShader(VertexInput input)
{
    PixelInput output;

    // Dont even know what this is for
    input.pos.w = 1.0f;

    // Vertex on model -> model in world -> world to camera -> camera to 2D
    output.pos = mul(input.pos, world_matrix);
    output.pos = mul(output.pos, view_matrix);
    output.pos = mul(output.pos, projection_matrix);

    // Whats a vertex care about colour IDK pass it on
    output.tex = input.tex;

    return output;
}
