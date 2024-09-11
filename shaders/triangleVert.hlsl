cbuffer UniformBufferObject : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

struct VertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 vertexColor : COLOR;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    float4x4 mat = mul(mul(proj, view), model);
    output.position = mul(mat, float4(input.position, 1.0));
    //output.position = float4(input.position, 1.0);
    output.vertexColor = input.color;
    return output;
}

// Notes:
// - SV = System Value
// - SV_POSITION is a system value that represents the position of a vertex in clip space