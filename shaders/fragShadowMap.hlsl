Texture2D shadowMap : register(t1);
SamplerState colorSampler : register(s2);

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

struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
};

float4 main(FragmentInput input) : SV_TARGET
{
    float depth = shadowMap.Sample(colorSampler, input.uv.xy).r;
    float4 color = depth * float4(1.0f, 1.0f, 1.0f, 1.0f);
    return color;
}