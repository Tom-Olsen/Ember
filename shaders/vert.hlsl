cbuffer UniformBufferObject : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};



struct PushConstant
{
    float4 time;
    float4 delaTime;
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
    float4x4 mat = mul(mul(proj, view), model);
    float4 pos = float4(input.position, 1.0);
    //pos.z = pos.z + sin(pc.time.y);
    
    VertexOutput output;
    output.position = mul(mat, pos);
    output.vertexColor = input.color;
    output.uv = input.uv;
    return output;
}

// Notes:
// - SV = System Value
// - SV_POSITION is a system value that represents the position of a vertex in clip space