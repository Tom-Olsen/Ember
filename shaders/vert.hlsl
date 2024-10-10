cbuffer UniformBufferObject : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};



struct PushConstant
{
    float4x4 transform;
    float4x4 projView;
};
#if defined(_DXC)
[[vk::push_constant]] CullPushConstants pc;
#else
[[vk::push_constant]] ConstantBuffer<PushConstant> pc;
#endif



struct VertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
    float4 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
};



VertexOutput main(VertexInput input)
{
    VertexOutput output;
    float4x4 mat = mul(mul(proj, view), model);
    //float4x4 mat = mul(pc.projView, pc.transform);
    output.position = mul(mat, float4(input.position, 1.0));
    //output.position = float4(input.position, 1.0);
    output.vertexColor = input.color;
    output.uv = input.uv;
    return output;
}

// Notes:
// - SV = System Value
// - SV_POSITION is a system value that represents the position of a vertex in clip space