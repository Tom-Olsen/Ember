#include "shadowMapping.hlsli"
SamplerState colorSampler : register(s10);
SamplerComparisonState shadowSampler : register(s11);
Texture2D colorTexture : register(t20);
Texture2D shadowMap : register(t21);



cbuffer LightData : register(b1)
{
    float4x4 worldToClipMatrix[2];  // world to light clip space matrix (projection * view)
    float4 directionIntensity[2];   // direction (xyz) and intensity (w) of the light
    float4 lightColor[2];           // light color
    bool receiveShadows;            // 0 = false, 1 = true
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
    
    // Lighting:
    float3 lightDir = directionIntensity[0].xyz;
    float diffuse = max(dot(normal, lightDir), 0.0);
    float ambient = 0.1f;
    float shadow = 0;
    if (receiveShadows == true)
    {
        for (int i = 0; i < 1; i++)
        shadow += Shadow(worldPos, worldToClipMatrix[i], shadowMap, shadowSampler);
    }
    else
        shadow = 1;
 
    // Shading:
    float4 color = colorTexture.Sample(colorSampler, uv);
    
    return (ambient + diffuse * shadow) * color;
}