// Globals
cbuffer MatrixBuffer
{
    matrix world_matrix;
    matrix view_matrix;
    matrix projection_matrix;
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
    output.pos = mul(input.pos, world_matrix);
    output.pos = mul(output.pos, view_matrix);
    output.pos = mul(output.pos, projection_matrix);

    // Whats a vertex care about colour IDK pass it on
    output.colour = input.colour;

    return output;
}
