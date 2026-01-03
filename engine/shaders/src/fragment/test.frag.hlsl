#include "defaultPushConstant.hlsli"
#include "descriptorSets.hlsli"
#include "fragmentShaderCommon.hlsli"
#include "math.hlsli"
#include "shadowMapping.hlsli"



// Material set:
Texture2D colorMap : register(t0, MATERIAL_SET);                 // format = VK_FORMAT_R8G8B8A8_SRGB,
Texture2D normalMap : register(t1, MATERIAL_SET);                // format = VK_FORMAT_R8G8B8A8_UNORM,   opengl style unorm normal map
Texture2D<float> metallicityMap : register(t2, MATERIAL_SET);    // format = VK_FORMAT_R8_UNORM,         single channel unorm metallicity map
Texture2D<float> roughnessMap : register(t3, MATERIAL_SET);      // format = VK_FORMAT_R8_UNORM,         single channel unorm roughness map



cbuffer SurfaceProperties : register(b4, MATERIAL_SET)
{
    float4 surface_diffuseColor;    // (1.0, 1.0, 1.0)
    float surface_roughness;        // 0.5
    float3 surface_reflectivity;    // 0.4
    float surface_metallicity;      // 0 = dielectric, 1 = metal
    float4 surface_scaleOffset;     // .xy = scale, .zw = offset
    bool surface_receiveShadows;    // 0 = false, 1 = true
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldTangent : TANGENT;      // tangent in world space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;              // texture coordinates
    float3 worldPosition : TEXCOORD1;   // position in world space
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float2 uv = input.uv.xy * surface_scaleOffset.xy + surface_scaleOffset.zw;
    float3 tangentSpaceNormal = normalize(input.worldNormal);
    float3 tangentSpaceTangent = normalize(LinAlg_VectorToPlaneProjection(input.worldTangent, tangentSpaceNormal));
    float3 tangentSpaceBitangent = cross(tangentSpaceNormal, tangentSpaceTangent);
    float3x3 TBN = transpose(float3x3(tangentSpaceTangent, tangentSpaceBitangent, tangentSpaceNormal));
    float4 vertexColor = input.vertexColor;
    float3 worldPos = input.worldPosition;
    
    // Shading:
    float4 color = vertexColor * surface_diffuseColor * colorMap.Sample(colorSampler, uv);
    float3 localNormal = 2.0 * normalMap.Sample(colorSampler, uv).xyz - 1.0;
    float3 worldNormal = normalize(mul(TBN, localNormal));
    float metallicity = surface_metallicity * metallicityMap.Sample(colorSampler, uv);
    float roughness = surface_roughness * roughnessMap.Sample(colorSampler, uv);
    
    // Lighting:
    float ambient = 0.1f;
    float3 finalColor = ambient * color.xyz;
    finalColor += PhysicalLighting(worldPos, worldNormal, color.xyz, roughness, surface_reflectivity, metallicity, surface_receiveShadows);
    
    return float4(finalColor, 1.0f);
}