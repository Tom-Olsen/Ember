#include "shadowMapping.hlsli"
SamplerComparisonState shadowSampler : register(s10);
Texture2D shadowMap : register(t20);



cbuffer LightData : register(b1)
{
    float4x4 worldToClipMatrix[2];  // world to light clip space matrix (projection * view)
    float4 directionIntensity[2];   // direction (xyz) and intensity (w) of the light
    float4 lightColor[2];           // light color
    bool receiveShadows;            // 0 = false, 1 = true
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
    float4 color = input.color;
    float3 normal = normalize(input.normal);
    float3 worldPos = input.worldPos;
    
    // Lighting:
    float3 lightDir = directionIntensity[0].xyz;
    float diffuse = max(dot(normal, lightDir), 0.0);
    float ambient = 0.1f;
    float shadow = receiveShadows == true ? Shadow(worldPos, worldToClipMatrix[0], shadowMap, shadowSampler) : 1;
    
    return (ambient + diffuse * shadow) * color;
}