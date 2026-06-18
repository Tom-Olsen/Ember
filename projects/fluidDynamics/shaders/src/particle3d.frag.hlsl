#include "fragmentShaderCommon.hlsli"
#include "shadowMapping.hlsli"



cbuffer SurfaceProperties : register(b300, CALL_SET)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
    float4 scaleOffset;     // .xy = scale, .zw = offset
    float3 reflectivity;    // 0.4
    float roughness;        // 0.5
    float metallicity;      // 0 = dielectric, 1 = metal
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldTangent : TANGENT;      // tangent in world space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;              // texture coordinates
    float3 worldPosition : TEXCOORD1;   // position in world space
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Shading:
    float4 color = input.vertexColor * diffuseColor;
    
    // Lighting:
    float ambient = 0.3f;
    float3 finalColor = ambient * color.xyz;
    finalColor += PhysicalLighting(input.worldPosition, input.worldNormal, color.xyz, roughness, reflectivity, metallicity, pc.receiveShadows != 0);
    
    return float4(finalColor, 1.0f);
}