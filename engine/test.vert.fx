// Globals
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Typedefs
struct VertexInputType
{
    float4 pos : POSITION;
    float4 colour : COLOUR;
};

struct PixelInputType
{
    float4 pos : SV_POSITION;
    float4 colour : COLOUR;
};

PixelInputType VertShader(VertexInputType input)
{
    PixelInputType output;

    // Dont even know what this is for
    input.pos.w = 1.0f;

    // Vertex on model -> model in world -> world to camera -> camera to 2D
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    // Whats a vertex care about colour IDK pass it on
    output.colour = input.colour;

    return output;
}
