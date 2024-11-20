#include "shadowMapping.hlsli"
SamplerComparisonState shadowSampler : register(s10);
Texture2DArray<float> shadowMaps : register(t25);



cbuffer LightData : register(b1)
{
    float4x4 worldToClipMatrix[MAX_D_LIGHTS];   // world to light clip space matrix (projection * view)
    float4 directionIntensity[MAX_D_LIGHTS];    // direction (xyz) and intensity (w) of the light
    float3 lightColor[MAX_D_LIGHTS];            // light color
    bool receiveShadows;                        // 0 = false, 1 = true
};



struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 worldPos : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float4 color = input.color;
    float3 normal = normalize(input.normal);
    float3 worldPos = input.worldPos;
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * color.xyz;
    finalColor += Shadow(worldPos, normal, color.xyz, worldToClipMatrix, directionIntensity, lightColor, shadowMaps, shadowSampler);
    
    return float4(finalColor, 1.0f);
}