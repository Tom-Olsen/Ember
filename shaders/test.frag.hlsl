#include "shadowMapping.hlsli"
SamplerState colorSampler : register(s10);
SamplerComparisonState shadowSampler : register(s11);
Texture2DArray<float> colorTextures : register(t24);
Texture2DArray<float> shadowMaps : register(t21);



cbuffer LightData : register(b1)
{
    float4x4 worldToClipMatrix[MAX_D_LIGHTS];   // world to light clip space matrix (projection * view)
    float4 directionIntensity[MAX_D_LIGHTS];    // direction (xyz) and intensity (w) of the light
    float3 lightColor[MAX_D_LIGHTS];            // light color
    bool receiveShadows;                        // 0 = false, 1 = true
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



struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float2 uv = input.uv.xy;
    float3 normal = normalize(input.normal);
    float3 worldPos = input.worldPos;
    
    // Shading:
    float index = 0.5f * (sin(pc.time.x) + 1) * (MAX_D_LIGHTS - 1.0f);
    float4 color = colorTextures.Sample(colorSampler, float3(uv, index));
   
    return color;
}