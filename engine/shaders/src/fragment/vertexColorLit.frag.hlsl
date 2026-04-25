#include "fragmentShaderCommon.hlsli"
#include "shadowMapping.hlsli"



cbuffer SurfaceProperties : register(b300, CALL_SET)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float4 vertexColor : COLOR;         // vertex color
    float3 worldPos : TEXCOORD0;        // position in world space
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float4 color = diffuseColor * input.vertexColor;
    float3 worldNormal = normalize(input.worldNormal);
    float3 worldPos = input.worldPos;
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * color.xyz;
    finalColor += PhysicalLighting(worldPos, worldNormal, color.xyz, 0.0f, 0.0f, 0.0f, pc.receiveShadows != 0);
    
    return float4(finalColor, 1.0f);
}