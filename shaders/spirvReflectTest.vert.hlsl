#include "linearAlgebra.hlsli"



cbuffer UniformBuffer : register(b0)
{
    bool  bool_;
    bool2 bool_2;
    int int_;
    int3 int_3;
    float  float_;
    float2 float_2;
    float3 float_3;
    float4 float_4;
    float2x2 float_22;
    float2x3 float_23;
    float2x4 float_24;
    float3x2 float_32;
    float3x3 float_33;
    float3x4 float_34;
    float4x2 float_42;
    float4x3 float_43;
    float4x4 float_44;
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
    float3 normal : NORMAL;
    float4 color : COLOR;
    float4 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};



VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.position = float_;
    output.normal = float3(float_, float_2);
    output.vertexColor = 0;
    output.uv = 0;
    output.worldPos = 0;
    return output;
}

// Notes:
// - SV = System Value
// - SV_POSITION is a system value that represents the position of a vertex in clip space