#include "fragmentShaderCommon.hlsli"
#include "shadowMapping.hlsli"



cbuffer SurfaceProperties : register(b2)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldPosition : TEXCOORD1;   // position in world space
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float3 worldPos = input.worldPosition;
    float3 worldNormal = normalize(input.worldNormal);
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * diffuseColor.xyz;
    finalColor += PhysicalLighting(worldPos, worldNormal, diffuseColor.xyz, 0.0f, 0.0f, 0.0f, pc.receiveShadows != 0);
    
    return float4(finalColor, 1.0f);
}